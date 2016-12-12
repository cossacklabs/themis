package cell

import (
	"bytes"
	"crypto/rand"
	"math/big"
	"testing"
)

func testProtect(mode int, context []byte, t *testing.T) {
	data_len, err := rand.Int(rand.Reader, big.NewInt(1024))
	if nil != err {
		t.Error(err)
	}

	data := make([]byte, int(data_len.Int64()))
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
	encData, addData, err := sc.Protect(data, context)
	if nil == err {
		t.Error("Scell encription with empty password ")
	}

	sc = New([]byte{}, mode)
	encData, addData, err = sc.Protect(data, context)
	if nil == err {
		t.Error("Scell encription with empty password ")
	}

	sc = New(key, mode)
	encData, addData, err = sc.Protect(nil, context)
	if nil == err {
		t.Error("Scell encrypt empty data")
	}

	encData, addData, err = sc.Protect([]byte{}, context)
	if nil == err {
		t.Error("Scell encrypt empty data")
	}

	encData, addData, err = sc.Protect(data, context)
	if nil != err {
		t.Error(err)
	}

	if 0 == bytes.Compare(data, encData) {
		t.Error("Original data and encrypted data match")
	}

	decData, err := sc.Unprotect(nil, addData, context)
	if nil == err {
		t.Error("Scell decrypt empty data")
	}

	decData, err = sc.Unprotect([]byte{}, addData, context)
	if nil == err {
		t.Error("Scell decrypt empty data")
	}
	decData, err = sc.Unprotect(encData, addData, context)

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

	testProtect(CELL_MODE_SEAL, nil, t)
	testProtect(CELL_MODE_SEAL, context, t)

	testProtect(CELL_MODE_TOKEN_PROTECT, nil, t)
	testProtect(CELL_MODE_TOKEN_PROTECT, context, t)

	testProtect(CELL_MODE_CONTEXT_IMPRINT, context, t)
}
