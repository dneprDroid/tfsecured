package tfsecured

import (
	"fmt"
	"io"
	"crypto/rand"
	"crypto/cipher"
	"crypto/aes"
)

const blockSize = 16

func decryptAES(key []byte, data []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err 
	}
	if len(data) < blockSize {
		return nil, fmt.Errorf("ciphertext too short: %v", len(data))
	}
	iv := data[:blockSize]

	if len(data) % blockSize != 0 {
		return nil, fmt.Errorf("ciphertext is not a multiple of the block size: %v", len(data))
	}
	result := make([]byte, len(data) - blockSize)

	mode := cipher.NewCBCDecrypter(block, iv)
	mode.CryptBlocks(result, data[blockSize:])

	result = result[:len(result)-3]
	return result, nil 
}

func encryptAES(key []byte, data []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	encryptedData := make([]byte, blockSize + len(data))
	if _, err = io.ReadFull(rand.Reader, encryptedData[:blockSize]); err != nil {
		return nil, err
	}
	stream := cipher.NewCBCDecrypter(block, encryptedData[:blockSize])
	stream.CryptBlocks(encryptedData[blockSize:], data)

	return encryptedData, nil
}