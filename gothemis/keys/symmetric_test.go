package keys

import (
	"testing"
)

const defaultLength = 32

func TestNewSymmetricKey(t *testing.T) {
	key, err := NewSymmetricKey()
	if err != nil {
		t.Error(err)
	}
	if len(key.Value) != defaultLength {
		t.Error("invalid key.Value")
	}
}
