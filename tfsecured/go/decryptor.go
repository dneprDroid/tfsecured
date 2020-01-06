package tfsecured

import (
	"io/ioutil"
	
	tf "github.com/tensorflow/tensorflow/tensorflow/go"
)

type Decryptor struct {
	key string, 
	buf []byte
}

func NewDecryptorBytes(key string, data []byte) *Decryptor {
	return &Decryptor{
		key: key, 
		buf: data, 
	}
}

func NewDecryptorFile(key string, path string) (*Decryptor, error) {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}
	return NewDecryptorBytes(key, data)
}

func (d *Decryptor) Decrypt() (*tf.Graph, error) {
	keyHash := hashSHA256([]byte(d.key))
	decryptedData, err := decryptAES(keyHash, d.buf)
	if err != nil {
		return nil, err 
	}
	graph := tf.NewGraph()
	if err := graph.Import(decryptedData, ""); err != nil {
		return nil, err 
	}
	return graph, nil 
}