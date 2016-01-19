package gothemis

import (
    "testing"
)

func TestNewKeypair(t *testing.T) {
	_, err := NewKeypair(KEYTYPE_EC)
	if nil != err {
		t.Error(err)
	}
	
	_, err = NewKeypair(KEYTYPE_RSA)
	if nil != err {
		t.Error(err)
	}
}

