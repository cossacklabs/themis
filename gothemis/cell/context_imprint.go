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

// SecureCellContextImprint is Secure Cell in Context Imprint mode.
//
// The data is protected by a secret symmetric key.
// Use keys.NewSymmetricKey() to generate keys suitable for Secure Cell.
//
// Context Imprint mode is intended for environments where storage constraints
// do not allow the size of the data to grow
// and there is no auxiliary storage available.
// Context Imprint mode requires an additional ``associated context''
// to be provided along with the secret in order to protect the data.
//
// In Context Imprint mode no authentication token is computed or verified.
// This means the integrity of the data is not enforced,
// so the overall security level is slightly lower than in Seal or Token Protect modes.
//
// To ensure highest security level possible,
// supply a different associated context for each encryption invocation with the same secret.
//
// Read more about Context Imprint mode here:
//
// https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#context-imprint-mode
type SecureCellContextImprint struct {
	key *keys.SymmetricKey
}

// ContextImprintWithKey makes a new Secure Cell in Context Imprint mode secured by a symmetric key.
func ContextImprintWithKey(key *keys.SymmetricKey) (*SecureCellContextImprint, error) {
	if key == nil || len(key.Value) == 0 {
		return nil, ErrMissingKey
	}
	return &SecureCellContextImprint{key}, nil
}

// Encrypt message.
//
// Message is encrypted and provided context is cryptographically mixed with the data,
// but is not included into the resulting encrypted message.
// You will have to provide the same context again during decryption.
// Usually this is some plaintext data associated with encrypted data,
// such as database row number, protocol message ID, etc.
//
// Encrypted data is returned as a single byte slice of the same size as input,
// it cannot be authenticated later.
//
// An error is returned on failure, such as if the message or context is empty,
// or in case of some internal failure in cryptographic backend.
func (sc *SecureCellContextImprint) Encrypt(message []byte, context []byte) ([]byte, error) {
	if len(message) == 0 {
		return nil, ErrMissingMessage
	}
	if len(context) == 0 {
		return nil, ErrMissingContext
	}

	length, err := encryptContextImprint(sc.key.Value, message, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	encrypted := make([]byte, length)
	length, err = encryptContextImprint(sc.key.Value, message, context, encrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	return encrypted[:length], nil
}

// Decrypt message.
//
// Secure Cell validates association with the context data and decrypts the message.
// You need to provide the same context as it was used during encryption.
//
// Non-empty decrypted data is returned if everything goes well.
//
// Note that data integrity is not verified by Context Imprint mode: garbage in — garbage out.
// If data has been corrupted, a different key is used, or the context is incorrect
// then Secure Cell will most likely successfully return corrupted output.
//
// An error will be returned on failure, such as if the message or context is empty,
// or in case of some internal failure in cryptographic backend.
func (sc *SecureCellContextImprint) Decrypt(encrypted []byte, context []byte) ([]byte, error) {
	if len(encrypted) == 0 {
		return nil, ErrMissingMessage
	}
	if len(context) == 0 {
		return nil, ErrMissingContext
	}

	length, err := decryptContextImprint(sc.key.Value, encrypted, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	decrypted := make([]byte, length)
	length, err = decryptContextImprint(sc.key.Value, encrypted, context, decrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	return decrypted[:length], nil
}

func encryptContextImprint(symmetricKey, plaintext, userContext, ciphertext []byte) (int, errors.ThemisErrorCode) {
	ciphertextLength := bytesSize(ciphertext)
	err := C.themis_secure_cell_encrypt_context_imprint(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(plaintext),
		bytesSize(plaintext),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(ciphertext),
		&ciphertextLength,
	)
	if sizeOverflow(ciphertextLength) {
		return 0, errors.NoMemory
	}
	return int(ciphertextLength), errors.ThemisErrorCode(err)
}

func decryptContextImprint(symmetricKey, ciphertext, userContext, plaintext []byte) (int, errors.ThemisErrorCode) {
	plaintextLength := bytesSize(plaintext)
	err := C.themis_secure_cell_decrypt_context_imprint(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(ciphertext),
		bytesSize(ciphertext),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(plaintext),
		&plaintextLength,
	)
	if sizeOverflow(plaintextLength) {
		return 0, errors.NoMemory
	}
	return int(plaintextLength), errors.ThemisErrorCode(err)
}
