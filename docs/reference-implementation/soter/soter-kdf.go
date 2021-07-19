package soter

import (
	"crypto/hmac"
	"crypto/sha256"
)

// SoterKDF derives a new key of requested length from an input key.
// Label indicates key derivation purpose.
// Derived key is also bound to provided context data (if any).
// Output key length must be between 1 and 32 bytes.
func SoterKDF(input []byte, label string, outputBytes int, context ...[]byte) []byte {
	// Respect limitations of HMAC-SHA-256.
	if outputBytes < 1 || outputBytes > 32 {
		panic("invalid output length")
	}
	// Derive 32 bytes of implicit key if the input key is not specified.
	// The key is a truncated XOR-sum of the label and all the context values.
	if len(input) == 0 {
		input = make([]byte, 32)
		copy(input, label)
		for _, context := range context {
			for i := 0; i < min(len(input), len(context)); i++ {
				input[i] ^= context[i]
			}
		}
	}
	// Derive output key with HMAC keyed by the input key, computing hash sum
	// of all the context data with appropriate delimiters between the pieces.
	mac := hmac.New(sha256.New, input)
	mac.Write([]byte{0x00, 0x00, 0x00, 0x01})
	mac.Write([]byte(label))
	mac.Write([]byte{0x00})
	for _, context := range context {
		mac.Write(context)
	}
	result := mac.Sum(nil)
	// Return the result truncated to desired output key size.
	return result[:outputBytes]
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
