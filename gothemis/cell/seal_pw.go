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
)

// SecureCellSealPassphrase is Secure Cell in Seal mode.
// This is the most secure and easy way to protect stored data.
//
// The data is protected by a secret passphrase.
// You can safely use relatively short, human-readable passwords with this mode.
//
// If you do not need to keep the secret in the head and would rather write it into a file,
// consider using symmetric keys.
// See ``cell.SealWithKey()'' and ``SecureCellSeal''.
//
// Read more about Seal mode here:
//
// https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#seal-mode
type SecureCellSealPassphrase struct {
	passphrase string
}

// SealWithPassphrase makes a new Secure Cell in Seal mode secured by a passphrase.
func SealWithPassphrase(passphrase string) (*SecureCellSealPassphrase, error) {
	if len(passphrase) == 0 {
		return nil, ErrMissingPassphrase
	}
	return &SecureCellSealPassphrase{passphrase}, nil
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
func (sc *SecureCellSealPassphrase) Encrypt(message, context []byte) ([]byte, error) {
	if len(message) == 0 {
		return nil, ErrMissingMessage
	}

	length, err := encryptSealWithPassphrase(sc.passphrase, message, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	encrypted := make([]byte, length)
	length, err = encryptSealWithPassphrase(sc.passphrase, message, context, encrypted)
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
func (sc *SecureCellSealPassphrase) Decrypt(encrypted, context []byte) ([]byte, error) {
	if len(encrypted) == 0 {
		return nil, ErrMissingMessage
	}

	length, err := decryptSealWithPassphrase(sc.passphrase, encrypted, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	decrypted := make([]byte, length)
	length, err = decryptSealWithPassphrase(sc.passphrase, encrypted, context, decrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	return decrypted[:length], nil
}

func encryptSealWithPassphrase(passphrase string, plaintext, userContext, ciphertext []byte) (int, errors.ThemisErrorCode) {
	ciphertextLength := bytesSize(ciphertext)
	err := C.themis_secure_cell_encrypt_seal_with_passphrase(
		bytesData([]byte(passphrase)),
		bytesSize([]byte(passphrase)),
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

func decryptSealWithPassphrase(passphrase string, ciphertext, userContext, plaintext []byte) (int, errors.ThemisErrorCode) {
	plaintextLength := bytesSize(plaintext)
	err := C.themis_secure_cell_decrypt_seal_with_passphrase(
		bytesData([]byte(passphrase)),
		bytesSize([]byte(passphrase)),
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
