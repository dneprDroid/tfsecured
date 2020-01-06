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

	stream := cipher.NewCBCDecrypter(block, iv)
	stream.CryptBlocks(result, data[blockSize:])
	return unpad(result), nil 
}

func encryptAES(key []byte, data []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	paddedData := pad(data, blockSize)
	encryptedData := make([]byte, blockSize + len(paddedData))
	if _, err = io.ReadFull(rand.Reader, encryptedData[:blockSize]); err != nil {
		return nil, err
	}
	stream := cipher.NewCBCDecrypter(block, encryptedData[:blockSize])
	stream.CryptBlocks(encryptedData[blockSize:], paddedData)

	return encryptedData, nil
}

func pad(data []byte, bs int) []byte {
	rval := bs - len(data) % bs
	suffix := make([]byte, rval, rval)
	return append(data, suffix...)
}

func unpad(data []byte) []byte {
	last := data[len(data) - 1]
	lastIdx := len(data)- int(last)
	if lastIdx >= len(data) {
		return data 
	}
	return data[:lastIdx]
}