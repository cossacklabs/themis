package keys

/*
#cgo LDFLAGS: -lthemis

#include <stdbool.h>
#include <themis/themis.h>

static bool get_sym_key_size(size_t *key_len)
{
	return themis_gen_sym_key(NULL, key_len) == THEMIS_BUFFER_TOO_SMALL;
}

static bool gen_sym_key(void *key, size_t key_len)
{
	return themis_gen_sym_key(key, &key_len) == THEMIS_SUCCESS;
}

*/
import "C"

import (
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
)

// Errors returned by key generation.
var (
	ErrGetSymmetricKeySize  = errors.New("failed to get symmetric key size")
	ErrGenerateSymmetricKey = errors.New("failed to generate symmetric key")
)

// SymmetricKey stores a master key for Secure Cell.
type SymmetricKey struct {
	Value []byte
}

// NewSymmetricKey generates a new random symmetric key.
func NewSymmetricKey() (*SymmetricKey, error) {
	var len C.size_t
	if !bool(C.get_sym_key_size(&len)) {
		return nil, ErrGetSymmetricKeySize
	}
	if sizeOverflow(len) {
		return nil, ErrOutOfMemory
	}

	key := make([]byte, int(len), int(len))
	if !bool(C.gen_sym_key(unsafe.Pointer(&key[0]), len)) {
		return nil, ErrGenerateSymmetricKey
	}

	return &SymmetricKey{Value: key}, nil
}
