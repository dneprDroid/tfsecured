package tfsecured

import (
	"log"
	"testing"

	tf "github.com/tensorflow/tensorflow/tensorflow/go"
)

func TestGraph(t *testing.T) {
	key := "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM"
	path := "../../python/demo/models/saved_model.pb"

	encryptor, err := NewEncryptorFile(key, path)
	if err != nil {
		t.Errorf("NewEncryptorFile: %v", err)
	}

	encryptedModel, err := encryptor.Encrypt();
	if err != nil {
		t.Errorf("Encrypt: %v", err)
	}

	decryptor := NewDecryptorBytes(key, encryptedModel)

	graph, err := decryptor.Decrypt(); 
	if err != nil {
		t.Errorf("Decrypt: %v", err)
	}
	log.Printf("graph: %v", graph)

	ops := graph.Operations()

	if len(ops) == 0 {
		t.Error("Graph doesn't contain operations")
	}

	session, err := tf.NewSession(graph, nil)
	if err != nil {
		t.Error("Graph is invalid")
	}
	session.Close()
}