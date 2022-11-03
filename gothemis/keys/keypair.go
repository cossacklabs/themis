package keys

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/themis_error.h>
#include <themis/secure_keygen.h>

#define KEYTYPE_EC 0
#define KEYTYPE_RSA 1

static bool get_key_size(int key_type, size_t *priv_len, size_t *pub_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (key_type)
	{
	case KEYTYPE_EC:
		res = themis_gen_ec_key_pair(NULL, priv_len, NULL, pub_len);
		break;
	case KEYTYPE_RSA:
		res = themis_gen_rsa_key_pair(NULL, priv_len, NULL, pub_len);
		break;
	default:
		return false;
	}

	return THEMIS_BUFFER_TOO_SMALL == res;
}

static bool gen_keys(int key_type, void *private, size_t priv_len, void *public, size_t pub_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (key_type)
	{
	case KEYTYPE_EC:
		res = themis_gen_ec_key_pair(private, &priv_len, public, &pub_len);
		break;
	case KEYTYPE_RSA:
		res = themis_gen_rsa_key_pair(private, &priv_len, public, &pub_len);
		break;
	default:
		return false;
	}

	return THEMIS_SUCCESS == res;
}

*/
import "C"

import (
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
)

// Type of Themis key.
const (
	TypeEC = iota
	TypeRSA
)

// Type of Themis key.
//
// Deprecated: Since 0.11. Use "keys.Type..." constants instead.
const (
	KEYTYPE_EC  = TypeEC
	KEYTYPE_RSA = TypeRSA
)

// Errors returned by key generation.
var (
	ErrGetKeySize       = errors.New("failed to get needed key sizes")
	ErrGenerateKeypair  = errors.New("failed to generate key pair")
	ErrInvalidType      = errors.NewWithCode(errors.InvalidParameter, "invalid key type specified")
	ErrOutOfMemory      = errors.NewWithCode(errors.NoMemory, "key generator cannot allocate enough memory")
	// Deprecated: Since 0.14. Use ErrOutOfMemory instead.
	ErrOverflow         = ErrOutOfMemory
)

// PrivateKey stores a ECDSA or RSA private key.
type PrivateKey struct {
	Value []byte
}

// PublicKey stores a ECDSA or RSA public key.
type PublicKey struct {
	Value []byte
}

// Keypair stores a ECDSA or RSA key pair.
type Keypair struct {
	Private *PrivateKey
	Public  *PublicKey
}

// New generates a new random pair of keys of the specified type.
func New(keytype int) (*Keypair, error) {
	if (keytype != TypeEC) && (keytype != TypeRSA) {
		return nil, ErrInvalidType
	}

	var privLen, pubLen C.size_t
	if !bool(C.get_key_size(C.int(keytype), &privLen, &pubLen)) {
		return nil, ErrGetKeySize
	}
	if sizeOverflow(privLen) || sizeOverflow(pubLen) {
		return nil, ErrOutOfMemory
	}

	priv := make([]byte, int(privLen), int(privLen))
	pub := make([]byte, int(pubLen), int(pubLen))

	if !bool(C.gen_keys(C.int(keytype), unsafe.Pointer(&priv[0]), privLen, unsafe.Pointer(&pub[0]), pubLen)) {
		return nil, ErrGenerateKeypair
	}

	return &Keypair{
		Private: &PrivateKey{Value: priv},
		Public:  &PublicKey{Value: pub},
	}, nil
}

// C returns sizes as size_t but Go expresses buffer lengths as int.
// Make sure that all sizes are representable in Go and there is no overflows.
func sizeOverflow(n C.size_t) bool {
	const maxInt = int(^uint(0) >> 1)
	return n > C.size_t(maxInt)
}
