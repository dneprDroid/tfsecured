package tfsecured

import (
	"io"
	"bytes"
	"crypto/rand"
	mathrand "math/rand"
	"testing"
)

const keySize = 32

func randomBytesWithSize(size int) ([]byte, error) {
	result := make([]byte, size)
	if _, err := io.ReadFull(rand.Reader, result); err != nil {
		return nil, err
	}
	return result, nil 
}

func randomBytesWithRange(minSize, maxSize int) ([]byte, error) {
	size := mathrand.Intn(maxSize - minSize) + minSize
	return randomBytesWithSize(size)
}

func TestEncryptionAES(t *testing.T) {
	key, err := randomBytesWithSize(keySize)
	if err != nil {
		t.Error(err)
	}
	data, err := randomBytesWithRange(100, 1000)
	if err != nil {
		t.Error(err)
	}
	_, err = encryptAES(key, data)
	if err != nil {
		t.Error(err)
	}
}

func TestEncryptionDecryptionAES(t *testing.T) {
	key, err := randomBytesWithSize(keySize)
	if err != nil {
		t.Error(err)
	}
	originData, err := randomBytesWithRange(100, 1000)
	if err != nil {
		t.Error(err)
	}
	encryptedData, err := encryptAES(key, originData)
	if err != nil {
		t.Error(err)
	}
	decryptedData, err := decryptAES(key, encryptedData)
	if err != nil {
		t.Error(err)
	}
	if !bytes.Equal(originData, decryptedData) {
		t.Errorf("Data slices aren't equal: `%v` \nand\n `%v`", originData, decryptedData)
	}
}