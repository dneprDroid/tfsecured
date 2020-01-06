package tfsecured

import (
	"bufio"
	"bytes"
	"io/ioutil"

	tf "github.com/tensorflow/tensorflow/tensorflow/go"
)

type Encryptor struct {
	key string 
	data []byte
}

func NewEncryptorGraph(key string, graph *tf.Graph) (*Encryptor, error) {
	var buff bytes.Buffer
    buffWriter := bufio.NewWriter(&buff)
	if _, err := graph.WriteTo(buffWriter); err != nil {
		return nil, err 
	}
	return NewEncryptorBytes(key, buff.Bytes()), nil 
}

func NewEncryptorFile(key string, path string) (*Encryptor, error) {
	data, err := ioutil.ReadFile(path)
	if err != nil {
		return nil, err
	}
	return NewEncryptorBytes(key, data), nil 
}

func NewEncryptorBytes(key string, data []byte) *Encryptor {
	return &Encryptor{
		key: key, 
		data: data, 
	}
}

func (e *Encryptor) Encrypt() ([]byte, error) {
	keyHash := hashSHA256([]byte(e.key))
	encryptedData, err := encryptAES(keyHash, e.data)
	if err != nil {
		return nil, err 
	}
	return encryptedData, nil 
}