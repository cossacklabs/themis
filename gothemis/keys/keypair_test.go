package keys

import (
	"testing"
)

func TestNewKeypair(t *testing.T) {
	_, err := New(KEYTYPE_EC)
	if nil != err {
		t.Error(err)
	}

	_, err = New(KEYTYPE_RSA)
	if nil != err {
		t.Error(err)
	}
}
