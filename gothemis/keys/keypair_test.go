package keys

import (
	"testing"
)

func TestNewKeypair(t *testing.T) {
	_, err := New(TypeEC)
	if nil != err {
		t.Error(err)
	}

	_, err = New(TypeRSA)
	if nil != err {
		t.Error(err)
	}
}
