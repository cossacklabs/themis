package keys

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/error.h>
#include <themis/secure_message.h>

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
	"github.com/cossacklabs/themis/gothemis/errors"
	"unsafe"
)

const (
	KEYTYPE_EC  = 0
	KEYTYPE_RSA = 1
)

type PrivateKey struct {
	Value []byte
}

type PublicKey struct {
	Value []byte
}

type Keypair struct {
	Private *PrivateKey
	Public  *PublicKey
}

func New(keytype int) (*Keypair, error) {
	if (keytype != KEYTYPE_EC) && (keytype != KEYTYPE_RSA) {
		return nil, errors.New("Incorrect key type")
	}

	var privLen, pubLen C.size_t
	if !bool(C.get_key_size(C.int(keytype), &privLen, &pubLen)) {
		return nil, errors.New("Failed to get needed key sizes")
	}

	priv := make([]byte, int(privLen), int(privLen))
	pub := make([]byte, int(pubLen), int(pubLen))

	if !bool(C.gen_keys(C.int(keytype), unsafe.Pointer(&priv[0]), privLen, unsafe.Pointer(&pub[0]), pubLen)) {
		return nil, errors.New("Failed to generate keypair")
	}

	return &Keypair{
		Private: &PrivateKey{Value: priv},
		Public:  &PublicKey{Value: pub},
	}, nil
}
