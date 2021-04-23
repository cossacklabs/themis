/*
 * Copyright (c) 2020 Cossack Labs Limited
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

package cell

/*
#cgo LDFLAGS: -lthemis
#include <themis/themis.h>
*/
import "C"

import (
	"github.com/cossacklabs/themis/gothemis/errors"
	"github.com/cossacklabs/themis/gothemis/keys"
)

// SecureCellSeal is Secure Cell in Seal mode.
// This is the most secure and easy way to protect stored data.
//
// The data is protected by a secret symmetric key.
// Use keys.NewSymmetricKey() to generate keys suitable for Secure Cell.
//
// Secure Cell in Seal mode will encrypt the data
// and append an ``authentication tag'' to it with auxiliary security information.
// This means that that size of the encrypted data will be larger than the original input.
//
// Additionally, it is possible to bind the encrypted data to some ``associated context''
// (for example, database row number).
// In this case decryption of the data with incorrect context will fail
// (even if the secret is correct and the data has not been tampered).
// This establishes cryptographically secure association between the protected data
// and the context in which it is used.
// With database row numbers, for example, this prevents the attacker
// from swapping encrypted password hashes in the database
// so the system will not accept credentials of a different user.
//
// Read more about Seal mode here:
//
// https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#seal-mode
type SecureCellSeal struct {
	key *keys.SymmetricKey
}

// SealWithKey makes a new Secure Cell in Seal mode secured by a symmetric key.
func SealWithKey(key *keys.SymmetricKey) (*SecureCellSeal, error) {
	if key == nil || len(key.Value) == 0 {
		return nil, ErrMissingKey
	}
	return &SecureCellSeal{key}, nil
}

// Encrypt message.
//
// Message is encrypted and authentication token is appended to it,
// forming a single sealed buffer.
//
// The context, if provided, is cryptographically mixed with the data,
// but is not included into the resulting encrypted message.
// You will have to provide the same context again during decryption.
// Usually this is some plaintext data associated with encrypted data,
// such as database row number, protocol message ID, etc.
// Empty and nil contexts are identical.
//
// Encrypted data is returned as a single byte slice.
//
// An error is returned on failure, such as if the message is empty,
// or in case of some internal failure in cryptographic backend.
func (sc *SecureCellSeal) Encrypt(message, context []byte) ([]byte, error) {
	if len(message) == 0 {
		return nil, ErrMissingMessage
	}

	length, err := encryptSeal(sc.key.Value, message, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	encrypted := make([]byte, length)
	length, err = encryptSeal(sc.key.Value, message, context, encrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	return encrypted[:length], nil
}

// Decrypt message.
//
// Secure Cell validates association with the context data, decrypts the message,
// and verifies its integrity using authentication data embedded into the message.
//
// You need to provide the same context as used during encryption.
// (If there was no context you can use empty or nil value).
//
// Non-empty decrypted data is returned if everything goes well.
//
// An error will be returned on failure, such as if the message is empty,
// or if the data has been tampered with,
// or if the secret or associated context do not match the ones used for encryption.
func (sc *SecureCellSeal) Decrypt(encrypted, context []byte) ([]byte, error) {
	if len(encrypted) == 0 {
		return nil, ErrMissingMessage
	}

	length, err := decryptSeal(sc.key.Value, encrypted, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	decrypted := make([]byte, length)
	length, err = decryptSeal(sc.key.Value, encrypted, context, decrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	return decrypted[:length], nil
}

func encryptSeal(symmetricKey, plaintext, userContext, ciphertext []byte) (int, errors.ThemisErrorCode) {
	ciphertextLength := bytesSize(ciphertext)
	err := C.themis_secure_cell_encrypt_seal(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(plaintext),
		bytesSize(plaintext),
		bytesData(ciphertext),
		&ciphertextLength,
	)
	if sizeOverflow(ciphertextLength) {
		return 0, errors.NoMemory
	}
	return int(ciphertextLength), errors.ThemisErrorCode(err)
}

func decryptSeal(symmetricKey, ciphertext, userContext, plaintext []byte) (int, errors.ThemisErrorCode) {
	plaintextLength := bytesSize(plaintext)
	err := C.themis_secure_cell_decrypt_seal(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(ciphertext),
		bytesSize(ciphertext),
		bytesData(plaintext),
		&plaintextLength,
	)
	if sizeOverflow(plaintextLength) {
		return 0, errors.NoMemory
	}
	return int(plaintextLength), errors.ThemisErrorCode(err)
}
