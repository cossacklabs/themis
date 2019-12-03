package keys

import (
	"testing"
)

func TestNewSymmetricKey(t *testing.T) {
	key, err := NewSymmetricKey()
	if err != nil {
		t.Error(err)
	}
	if len(key.Value) == 0 {
		t.Error("empty key.Value")
	}
}
