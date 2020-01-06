package tfsecured

import (
	"crypto/sha256"
)

func hashSHA256(data []byte) []byte {
	hash := sha256.Sum256(data)
	return hash[:]
}