// go test -v
package tfsecured

import (
	"io/ioutil"
	"testing"

	tf "github.com/tensorflow/tensorflow/tensorflow/go"
)

func TestNormalModel(t *testing.T) {
	path := "../../python/demo/models/saved_model.pb"

	data, err := ioutil.ReadFile(path)

	if err != nil {
		t.Errorf("ReadFile: %v", err)
	}
	graph := tf.NewGraph()
	if err := graph.Import(data, ""); err != nil {
		t.Errorf("Import: %v", err)
	}
}

func TestDecryption(t *testing.T) {
	key := "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM"
	path := "../../python/demo/models/saved_model-encrypted.pb"

	decryptor, err := NewDecryptorFile(key, path)
	if err != nil {
		t.Errorf("NewDecryptorFile: %v", err)
	}
	if _, err := decryptor.Decrypt(); err != nil {
		t.Errorf("Decrypt: %v", err)
	}
}