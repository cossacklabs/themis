package soter

import (
	"encoding/binary"
	"testing"

	"github.com/cossacklabs/themis/docs/reference-implementation/soter"
	"github.com/cossacklabs/themis/gothemis/cell"
	"github.com/cossacklabs/themis/gothemis/keys"
)

// Secure Cell header starts with a Soter algorithm ID.

func TestSecureCellHeader(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal(err)
	}
	cell, err := cell.SealWithKey(key)
	if err != nil {
		t.Fatal(err)
	}
	data, err := cell.Encrypt([]byte("message"), nil)
	if err != nil {
		t.Fatal(err)
	}
	id := soter.AlgorithmID(binary.LittleEndian.Uint32(data[0:4]))
	if id.Algorithm() != soter.AesGCM {
		t.Error("unexpected algorithm:", id.Algorithm())
	}
	if id.KDF() != soter.NoKDF {
		t.Error("unexpected KDF:", id.KDF())
	}
	if id.Padding() != soter.PKCS7Padding {
		t.Error("unexpected padding:", id.Padding())
	}
	if id.KeyBits() != 256 {
		t.Error("unexpected key length:", id.KeyBits())
	}
}
