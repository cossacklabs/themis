package cell

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/themis_error.h>
#include <themis/secure_cell.h>

#define MODE_SEAL 0
#define MODE_TOKEN_PROTECT 1
#define MODE_CONTEXT_IMPRINT 2

static bool get_protect_size(const void *key, size_t key_len, const void *data, size_t data_len, const void *context, size_t context_len, int mode, size_t *enc_len, size_t *add_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (mode)
	{
	case MODE_SEAL:
		res = themis_secure_cell_encrypt_seal(key, key_len, context, context_len, data, data_len, NULL, enc_len);
		break;
	case MODE_TOKEN_PROTECT:
		res = themis_secure_cell_encrypt_token_protect(key, key_len, context, context_len, data, data_len, NULL, add_len, NULL, enc_len);
		break;
	case MODE_CONTEXT_IMPRINT:
		if (!context)
		{
			break;
		}

		res = themis_secure_cell_encrypt_context_imprint(key, key_len, data, data_len, context, context_len, NULL, enc_len);
		break;
	}

	return THEMIS_BUFFER_TOO_SMALL == res;
}

static bool encrypt(const void *key, size_t key_len, const void *data, size_t data_len, const void *context, size_t context_len, int mode, void *enc, size_t enc_len, void *add, size_t add_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (mode)
	{
	case MODE_SEAL:
		res = themis_secure_cell_encrypt_seal(key, key_len, context, context_len, data, data_len, enc, &enc_len);
		break;
	case MODE_TOKEN_PROTECT:
		res = themis_secure_cell_encrypt_token_protect(key, key_len, context, context_len, data, data_len, add, &add_len, enc, &enc_len);
		break;
	case MODE_CONTEXT_IMPRINT:
		if (!context)
		{
			break;
		}

		res = themis_secure_cell_encrypt_context_imprint(key, key_len, data, data_len, context, context_len, enc, &enc_len);
		break;
	}

	return THEMIS_SUCCESS == res;
}

static bool get_unprotect_size(const void *key, size_t key_len, const void *prot, size_t prot_len, const void *add, size_t add_len, const void *context, size_t context_len, int mode, size_t *dec_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (mode)
	{
	case MODE_SEAL:
		res = themis_secure_cell_decrypt_seal(key, key_len, context, context_len, prot, prot_len, NULL, dec_len);
		break;
	case MODE_TOKEN_PROTECT:
		if (!add)
		{
			break;
		}

		res = themis_secure_cell_decrypt_token_protect(key, key_len, context, context_len, prot, prot_len, add, add_len, NULL, dec_len);
		break;
	case MODE_CONTEXT_IMPRINT:
		if (!context)
		{
			break;
		}

		res = themis_secure_cell_encrypt_context_imprint(key, key_len, prot, prot_len, context, context_len, NULL, dec_len);
		break;
	}

	return THEMIS_BUFFER_TOO_SMALL == res;
}

static bool decrypt(const void *key, size_t key_len, const void *prot, size_t prot_len, const void *add, size_t add_len, const void *context, size_t context_len, int mode, void *dec, size_t dec_len)
{
	themis_status_t res = THEMIS_FAIL;

	switch (mode)
	{
	case MODE_SEAL:
		res = themis_secure_cell_decrypt_seal(key, key_len, context, context_len, prot, prot_len, dec, &dec_len);
		break;
	case MODE_TOKEN_PROTECT:
		if (!add)
		{
			break;
		}

		res = themis_secure_cell_decrypt_token_protect(key, key_len, context, context_len, prot, prot_len, add, add_len, dec, &dec_len);
		break;
	case MODE_CONTEXT_IMPRINT:
		if (!context)
		{
			break;
		}

		res = themis_secure_cell_encrypt_context_imprint(key, key_len, prot, prot_len, context, context_len, dec, &dec_len);
		break;
	}

	return THEMIS_SUCCESS == res;
}

*/
import "C"
import (
	"github.com/cossacklabs/themis/gothemis/errors"
	"unsafe"
)

// Secure Cell operation mode.
const (
	ModeSeal = iota
	ModeTokenProtect
	ModeContextImprint
)

// Secure Cell operation mode.
//
// Deprecated: Since 0.11. Use "cell.Mode..." constants instead.
const (
	CELL_MODE_SEAL            = ModeSeal
	CELL_MODE_TOKEN_PROTECT   = ModeTokenProtect
	CELL_MODE_CONTEXT_IMPRINT = ModeContextImprint
)

// SecureCell is a high-level cryptographic service aimed at protecting arbitrary data
// stored in various types of storage
type SecureCell struct {
	key  []byte
	mode int
}

// New makes a new Secure Cell with master key and specified mode.
func New(key []byte, mode int) *SecureCell {
	return &SecureCell{key, mode}
}

func missing(data []byte) bool {
	return data == nil || len(data) == 0
}

// Protect encrypts or signs data with optional user context (depending on the Cell mode).
func (sc *SecureCell) Protect(data []byte, context []byte) ([]byte, []byte, error) {
	if (sc.mode < ModeSeal) || (sc.mode > ModeContextImprint) {
		return nil, nil, errors.New("Invalid mode specified")
	}

	if missing(sc.key) {
		return nil, nil, errors.New("Master key was not provided")
	}

	if missing(data) {
		return nil, nil, errors.New("Data was not provided")
	}

	if ModeContextImprint == sc.mode {
		if missing(context) {
			return nil, nil, errors.New("Context is mandatory for context imprint mode")
		}
	}

	var ctx unsafe.Pointer
	var ctxLen C.size_t

	if !missing(context) {
		ctx = unsafe.Pointer(&context[0])
		ctxLen = C.size_t(len(context))
	}

	var encLen, addLen C.size_t

	if !bool(C.get_protect_size(unsafe.Pointer(&sc.key[0]),
		C.size_t(len(sc.key)),
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		ctx,
		ctxLen,
		C.int(sc.mode),
		&encLen,
		&addLen)) {
		return nil, nil, errors.New("Failed to get output size")
	}

	var addData []byte
	var add unsafe.Pointer

	encData := make([]byte, encLen, encLen)
	if addLen > 0 {
		addData = make([]byte, addLen, addLen)
		add = unsafe.Pointer(&addData[0])
	}

	if !bool(C.encrypt(unsafe.Pointer(&sc.key[0]),
		C.size_t(len(sc.key)),
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		ctx,
		ctxLen,
		C.int(sc.mode),
		unsafe.Pointer(&encData[0]),
		encLen,
		add,
		addLen)) {
		return nil, nil, errors.New("Failed to protect data")
	}

	return encData, addData, nil
}

// Unprotect decrypts or verify data with optional user context (depending on the Cell mode).
func (sc *SecureCell) Unprotect(protectedData []byte, additionalData []byte, context []byte) ([]byte, error) {
	if (sc.mode < ModeSeal) || (sc.mode > ModeContextImprint) {
		return nil, errors.New("Invalid mode specified")
	}

	if missing(sc.key) {
		return nil, errors.New("Master key was not provided")
	}

	if missing(protectedData) {
		return nil, errors.New("Data was not provided")
	}

	if ModeContextImprint == sc.mode {
		if missing(context) {
			return nil, errors.New("Context is mandatory for context imprint mode")
		}
	}

	if ModeTokenProtect == sc.mode {
		if missing(additionalData) {
			return nil, errors.New("Additional data is mandatory for token protect mode")
		}
	}

	var add, ctx unsafe.Pointer = nil, nil
	var addLen, ctxLen C.size_t = 0, 0

	if !missing(additionalData) {
		add = unsafe.Pointer(&additionalData[0])
		addLen = C.size_t(len(additionalData))
	}

	if !missing(context) {
		ctx = unsafe.Pointer(&context[0])
		ctxLen = C.size_t(len(context))
	}

	var decLen C.size_t
	if !bool(C.get_unprotect_size(unsafe.Pointer(&sc.key[0]),
		C.size_t(len(sc.key)),
		unsafe.Pointer(&protectedData[0]),
		C.size_t(len(protectedData)),
		add,
		addLen,
		ctx,
		ctxLen,
		C.int(sc.mode),
		&decLen)) {
		return nil, errors.New("Failed to get output size")
	}

	decData := make([]byte, decLen, decLen)
	if !bool(C.decrypt(unsafe.Pointer(&sc.key[0]),
		C.size_t(len(sc.key)),
		unsafe.Pointer(&protectedData[0]),
		C.size_t(len(protectedData)),
		add,
		addLen,
		ctx,
		ctxLen,
		C.int(sc.mode),
		unsafe.Pointer(&decData[0]),
		decLen)) {
		return nil, errors.New("Failed to unprotect data")
	}

	return decData, nil
}
