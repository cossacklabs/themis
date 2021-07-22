package soter

import (
	"bytes"
	"encoding/hex"
	"testing"

	"github.com/cossacklabs/themis/docs/reference-implementation/soter"
)

const label = "Example key derivation"

func TestExplicitKey(t *testing.T) {
	inputKey, _ := hex.DecodeString(
		"4e6f68365577616568696564316b696a6f74686168326f506f68306565517565",
	)
	referenceOutput := soter.SoterKDF(inputKey, label, 32, []byte("2020-12-20"), []byte("11:18:24"))
	realSoterOutput := RealSoterKDF(inputKey, label, 32, []byte("2020-12-20"), []byte("11:18:24"))

	expectedOutput, _ := hex.DecodeString(
		"d5f5be45fd6eab6dcbf93c21c3d2d1e3e888fa20ef38f2f4a121c196382342dd",
	)
	if !bytes.Equal(referenceOutput, expectedOutput) {
		t.Error("invalid Soter KDF output (reference implementation)")
		t.Log("expected:", hex.EncodeToString(expectedOutput))
		t.Log("actual:  ", hex.EncodeToString(referenceOutput))
	}
	if !bytes.Equal(realSoterOutput, expectedOutput) {
		t.Error("invalid Soter KDF output (actual Soter library)")
		t.Log("expected:", hex.EncodeToString(expectedOutput))
		t.Log("actual:  ", hex.EncodeToString(realSoterOutput))
	}
}

func TestImplicitKey(t *testing.T) {
	referenceOutput := soter.SoterKDF(nil, label, 32, []byte("2020-12-20"), []byte("11:18:24"))
	realSoterOutput := RealSoterKDF(nil, label, 32, []byte("2020-12-20"), []byte("11:18:24"))

	expectedOutput, _ := hex.DecodeString(
		"cf9846b8026c5b76a0641aa85f4152ff02c15ad45b726c6e578be52afdfd6930",
	)
	if !bytes.Equal(referenceOutput, expectedOutput) {
		t.Error("invalid Soter KDF output (reference implementation)")
		t.Log("expected:", hex.EncodeToString(expectedOutput))
		t.Log("actual:  ", hex.EncodeToString(referenceOutput))
	}
	if !bytes.Equal(realSoterOutput, expectedOutput) {
		t.Error("invalid Soter KDF output (actual Soter library)")
		t.Log("expected:", hex.EncodeToString(expectedOutput))
		t.Log("actual:  ", hex.EncodeToString(realSoterOutput))
	}
}
