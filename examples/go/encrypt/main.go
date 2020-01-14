package main

import (
	"log"

	tfsecured "github.com/dneprDroid/tfsecured/tfsecured/go"
)

func main() {
	key := "BXKE0351PD9TXZ7XA8CK8XZU8XBGDM"
	path := "../../../python/demo/models/saved_model.pb"

	encryptor, err := tfsecured.NewEncryptorFile(key, path)
	if err != nil {
		log.Fatalf("NewEncryptorFile: %v", err)
	}

	if _, err := encryptor.Encrypt(); err != nil {
		log.Fatalf("Encrypt: %v", err)
	}

	log.Printf("Model was loaded successfully")
}