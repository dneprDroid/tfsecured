package main 

import (
	"log"
	"flag"
	"io/ioutil"

	tf "github.com/tensorflow/tensorflow/tensorflow/go"
	"github.com/tensorflow/tensorflow/tensorflow/go/op"

	tfsecured "github.com/dneprDroid/tfsecured/tfsecured/go"
)

func makeTensorFromImage(filename string) (*tf.Tensor, error) {
	bytes, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, err
	}
	tensor, err := tf.NewTensor(string(bytes))
	if err != nil {
		return nil, err
	}
	graph, input, output, err := constructGraphForImage(28, 28, 0, 1)
	if err != nil {
		return nil, err
	}
	session, err := tf.NewSession(graph, nil)
	if err != nil {
		return nil, err
	}
	defer session.Close()
	normalized, err := session.Run(
		map[tf.Output]*tf.Tensor{input: tensor},
		[]tf.Output{output},
		nil)
	if err != nil {
		return nil, err
	}
	return normalized[0], nil
}

func constructGraphForImage(
	w int32, h int32, mean float32, scale float32,
) (graph *tf.Graph, input, output tf.Output, err error) {	

	s := op.NewScope()
	input = op.Placeholder(s, tf.String)

	scaled := op.ResizeBilinear(s,
		op.ExpandDims(s,
			op.Cast(s,
				op.DecodeJpeg(s, input, op.DecodeJpegChannels(1)), tf.Float),
			op.Const(s.SubScope("make_batch"), int32(0))),
		op.Const(s.SubScope("size"), []int32{h, w}),
	)
	normalized := op.Div(s,
		op.Sub(s,
			scaled,
			op.Const(s.SubScope("mean"), mean)),
		op.Const(s.SubScope("scale"), scale),
	)
	output = op.Reshape(s,
		op.Transpose(s,
			normalized,
			op.Const(s.SubScope("perm"), []int32{0, 3, 1, 2}),
		),
		op.Const(s.SubScope("shape"), []int32{1, w * h}),
	)
	graph, err = s.Finalize()
	return graph, input, output, err
}

func main() {
	imagefile := flag.String("image", "", "Path to JPEG-image")
	path := flag.String("model", "", "Path to the encrypted model")
	key := flag.String("key", "", "Encryption key")

	flag.Parse()

	decryptor, err := tfsecured.NewDecryptorFile(*key, *path)
	if err != nil {
		log.Fatalf("NewDecryptorFile failed: %v", err)
	}
	graph, err := decryptor.Decrypt()
	if err != nil {
		log.Fatalf("Decrypt failed: %v", err)
	}
	session, err := tf.NewSession(graph, nil)
	if err != nil {
		log.Fatalf("NewSession failed: %v", err)
	}
	defer session.Close()

	tensor, err := makeTensorFromImage(*imagefile)
	if err != nil {
		log.Fatalf("makeTensorFromImage failed: %v", err)
	}

	output, err := session.Run(
		map[tf.Output]*tf.Tensor{
			graph.Operation("nn_input").Output(0): tensor,
		},
		[]tf.Output{
			graph.Operation("nn_output").Output(0),
		},
		nil)
	if err != nil {
		log.Fatalf("session failed: %v", err)
	}
	log.Printf("out: %v", output[0])

	outTensor := output[0].Value()
	outValue, ok := outTensor.([][]float32)
	if !ok {
		log.Fatalf("Invalid dimension of tensor: %v", outTensor)
	}

	detectedDigit := -1
	maxProb := float32(-1) 
	for i, prob := range outValue[0] {
		log.Printf("Probability for digit %v = %v", i+1, prob)
		if maxProb < prob {
			detectedDigit = i + 1
			maxProb = prob
		}
	}
	log.Printf("Detected digit %v", detectedDigit)
}