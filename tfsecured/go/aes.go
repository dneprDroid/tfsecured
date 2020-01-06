package tfsecured

import (
	"fmt"
	"io"
	"crypto/rand"
	"crypto/cipher"
	"crypto/aes"
)

func decryptAES(key []byte, data []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	if len(data) < aes.BlockSize {
		return nil, fmt.Errorf("Invalid data block size: %v", len(data))
	}
	iv := data[:aes.BlockSize]
	msgData := data[aes.BlockSize:]

	stream := cipher.NewCFBDecrypter(block, iv)
	stream.XORKeyStream(msgData, msgData)

	return msgData, nil
}

func encryptAES(key []byte, data []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}

	encryptedData := make([]byte, aes.BlockSize + len(data))
	iv := encryptedData[:aes.BlockSize]
	if _, err = io.ReadFull(rand.Reader, iv); err != nil {
		return nil, err
	}

	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(encryptedData[aes.BlockSize:], data)

	return encryptedData, nil
}