package soter

import "testing"

func TestExampleAlgorithmID(t *testing.T) {
	// Default value used by Secure Cell
	algorithmID := AlgorithmID(0x40010100)

	if algorithmID.Algorithm() != AesGCM {
		t.Error("unexpected algorithm")
	}
	if algorithmID.KDF() != NoKDF {
		t.Error("unexpected KDF")
	}
	if algorithmID.Padding() != PKCS7Padding {
		t.Error("unexpected padding")
	}
	if algorithmID.KeyBits() != 256 {
		t.Error("unexpected key size")
	}
}
