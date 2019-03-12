package cell

import (
	"bytes"
	"crypto/rand"
	"math/big"
	"testing"
)

func testProtect(mode int, context []byte, t *testing.T) {
	dataLen, err := rand.Int(rand.Reader, big.NewInt(1024))
	if nil != err {
		t.Error(err)
	}
	size := dataLen.Int64()
	if size == 0 {
		size = 1
	}

	data := make([]byte, size)
	_, err = rand.Read(data)
	if nil != err {
		t.Error(err)
	}

	key := make([]byte, 32)
	_, err = rand.Read(key)
	if nil != err {
		t.Error(err)
	}

	sc := New(nil, mode)
	_, _, err = sc.Protect(data, context)
	if nil == err {
		t.Error("Scell encryption with empty password ")
	}

	sc = New([]byte{}, mode)
	_, _, err = sc.Protect(data, context)
	if nil == err {
		t.Error("Scell encryption with empty password ")
	}

	sc = New(key, mode)
	_, _, err = sc.Protect(nil, context)
	if nil == err {
		t.Error("Scell encrypt empty data")
	}

	_, _, err = sc.Protect([]byte{}, context)
	if nil == err {
		t.Error("Scell encrypt empty data")
	}

	encData, addData, err := sc.Protect(data, context)
	if nil != err {
		t.Error(err)
	}

	if 0 == bytes.Compare(data, encData) {
		t.Error("Original data and encrypted data match")
	}

	_, err = sc.Unprotect(nil, addData, context)
	if nil == err {
		t.Error("Scell decrypt empty data")
	}

	_, err = sc.Unprotect([]byte{}, addData, context)
	if nil == err {
		t.Error("Scell decrypt empty data")
	}

	decData, err := sc.Unprotect(encData, addData, context)
	if nil != err {
		t.Error(err)
	}

	if 0 != bytes.Compare(data, decData) {
		t.Error("Original data and decrypted do not match")
	}
}

func TestProtect(t *testing.T) {
	context := make([]byte, 32)
	_, err := rand.Read(context)
	if nil != err {
		t.Error(err)
	}

	testProtect(ModeSeal, nil, t)
	testProtect(ModeSeal, context, t)

	testProtect(ModeTokenProtect, nil, t)
	testProtect(ModeTokenProtect, context, t)

	testProtect(ModeContextImprint, context, t)
}
