package main

import (
	"log"

	tfsecured "github.com/dneprDroid/tfsecured/tfsecured/go"
)

func main() {
	key := "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM"
	path := "../../../python/demo/models/saved_model-encrypted.pb"

	decryptor, err := tfsecured.NewDecryptorFile(key, path)
	if err != nil {
		log.Fatalf("NewDecryptorFile: %v", err)
	}

	if _, err := decryptor.Decrypt(); err != nil {
		log.Fatalf("Decrypt: %v", err)
	}

	log.Printf("Model was loaded successfully")
}