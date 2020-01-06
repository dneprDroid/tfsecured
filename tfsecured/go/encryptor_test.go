// go test -v
package tfsecured

import (
	"testing"
)

func TestEncryption(t *testing.T) {
	key := "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM"
	path := "../../python/demo/models/saved_model.pb"

	encryptor, err := NewEncryptorFile(key, path)
	if err != nil {
		t.Errorf("NewEncryptorFile: %v", err)
	}
	if _, err := encryptor.Encrypt(); err != nil {
		t.Errorf("Encrypt: %v", err)
	}
}