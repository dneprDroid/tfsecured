package tfsecured

import (
	"fmt"
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