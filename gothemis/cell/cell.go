/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Package cell provides Themis Secure Cell.
//
// Secure Сell is a high-level cryptographic service,
// aimed to protect arbitrary data being stored in various types of storages
// (like databases, filesystem files, document archives, cloud storage etc).
// It provides a simple way to secure your data using strong encryption
// and data authentication mechanisms,
// with easy-to-use interfaces for broad range of use-cases.
//
// Implementing secure storage is often constrained by various practical matters –
// ability to store keys, existence of length-sensitive code bound to database structure,
// requirements to preserve structure.
// To cover a broader range of usage scenarios and provide highest security level
// for systems with such constraints, we’ve designed several types of interfaces
// and implementations of secure data container, Secure Cell.
// They slightly differ in overall security level and ease of use:
// more complicated and slightly less secure ones can cover more constrained environments though.
// Interfaces below are prioritized by our preference,
// which takes only security and ease of use into account.
//
// ``SecureCellSeal'' is the most secure and the easiest one to use.
//
// ``SecureCellTokenProtect'' is able to preserve the encrypted data length
// but requires separate data storage to be available.
//
// ``SecureCellContextImprint'' preserves encrypted data length too,
// but at a cost of slightly lower security and more involved interface.
//
// Read more about Secure Cell modes here:
//
// https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/
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
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
)

// Errors returned by Secure Cell.
var (
	ErrGetOutputSize     = errors.New("failed to get output size")
	ErrEncryptData       = errors.New("failed to protect data")
	ErrDecryptData       = errors.New("failed to unprotect data")
	ErrInvalidMode       = errors.NewWithCode(errors.InvalidParameter, "invalid Secure Cell mode specified")
	ErrMissingKey        = errors.NewWithCode(errors.InvalidParameter, "empty symmetric key for Secure Cell")
	ErrMissingPassphrase = errors.NewWithCode(errors.InvalidParameter, "empty passphrase for Secure Cell")
	ErrMissingMessage    = errors.NewWithCode(errors.InvalidParameter, "empty message for Secure Cell")
	ErrMissingToken      = errors.NewWithCode(errors.InvalidParameter, "authentication token is required in Token Protect mode")
	ErrMissingContext    = errors.NewWithCode(errors.InvalidParameter, "associated context is required in Context Imprint mode")
	ErrOutOfMemory       = errors.NewWithCode(errors.NoMemory, "Secure Cell cannot allocate enough memory")
	// Deprecated: Since 0.14. Use ErrOutOfMemory instead.
	ErrOverflow          = ErrOutOfMemory
)

// Secure Cell operation mode.
//
// Deprecated: Since 0.13. Use SealWithKey(), TokenProtectWithKey(), ContextImprintWithKey() constructutors instead.
const (
	ModeSeal = iota
	ModeTokenProtect
	ModeContextImprint
)

// Secure Cell operation mode.
//
// Deprecated: Since 0.13. Use SealWithKey(), TokenProtectWithKey(), ContextImprintWithKey() constructutors instead.
const (
	CELL_MODE_SEAL            = ModeSeal
	CELL_MODE_TOKEN_PROTECT   = ModeTokenProtect
	CELL_MODE_CONTEXT_IMPRINT = ModeContextImprint
)

// SecureCell is a high-level cryptographic service aimed at protecting arbitrary data
// stored in various types of storage
//
// Deprecated: Since 0.13. Use SecureCellSeal, SecureCellTokenProtect, SecureCellContextImprint instead.
type SecureCell struct {
	key  []byte
	mode int
}

// New makes a new Secure Cell with master key and specified mode.
//
// Deprecated: Since 0.13. Use SealWithKey(), TokenProtectWithKey(), ContextImprintWithKey() constructutors instead.
func New(key []byte, mode int) *SecureCell {
	return &SecureCell{key, mode}
}

func missing(data []byte) bool {
	return data == nil || len(data) == 0
}

// C returns sizes as size_t but Go expresses buffer lengths as int.
// Make sure that all sizes are representable in Go and there is no overflows.
func sizeOverflow(n C.size_t) bool {
	const maxInt = int(^uint(0) >> 1)
	return n > C.size_t(maxInt)
}

// Protect encrypts or signs data with optional user context (depending on the Cell mode).
func (sc *SecureCell) Protect(data []byte, context []byte) ([]byte, []byte, error) {
	if (sc.mode < ModeSeal) || (sc.mode > ModeContextImprint) {
		return nil, nil, ErrInvalidMode
	}

	if missing(sc.key) {
		return nil, nil, ErrMissingKey
	}

	if missing(data) {
		return nil, nil, ErrMissingMessage
	}

	if ModeContextImprint == sc.mode {
		if missing(context) {
			return nil, nil, ErrMissingContext
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
		return nil, nil, ErrGetOutputSize
	}
	if sizeOverflow(encLen) || sizeOverflow(addLen) {
		return nil, nil, ErrOutOfMemory
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
		return nil, nil, ErrEncryptData
	}

	return encData, addData, nil
}

// Unprotect decrypts or verify data with optional user context (depending on the Cell mode).
func (sc *SecureCell) Unprotect(protectedData []byte, additionalData []byte, context []byte) ([]byte, error) {
	if (sc.mode < ModeSeal) || (sc.mode > ModeContextImprint) {
		return nil, ErrInvalidMode
	}

	if missing(sc.key) {
		return nil, ErrMissingKey
	}

	if missing(protectedData) {
		return nil, ErrMissingMessage
	}

	if ModeContextImprint == sc.mode {
		if missing(context) {
			return nil, ErrMissingContext
		}
	}

	if ModeTokenProtect == sc.mode {
		if missing(additionalData) {
			return nil, ErrMissingToken
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
		return nil, ErrGetOutputSize
	}
	if sizeOverflow(decLen) {
		return nil, ErrOutOfMemory
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
		return nil, ErrDecryptData
	}

	return decData, nil
}

func bytesData(b []byte) *C.uint8_t {
	if len(b) == 0 {
		return nil
	}
	return (*C.uint8_t)(unsafe.Pointer(&b[0]))
}

func bytesSize(b []byte) C.size_t {
	return C.size_t(len(b))
}
