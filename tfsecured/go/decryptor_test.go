// go test -v
package tfsecured

import (
	"testing"
)

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