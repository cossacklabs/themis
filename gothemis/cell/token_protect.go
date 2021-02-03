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

// SecureCellTokenProtect is Secure Cell in Token Protect mode.
//
// The data is protected by a secret symmetric key.
// Use keys.NewSymmetricKey() to generate keys suitable for Secure Cell.
//
// Token Protect mode is designed for cases when underlying storage constraints
// do not allow the size of the data to grow (so Seal mode cannot be used).
// However, if you have access to a different storage location
// (e.g., another table in the database)
// where additional security parameters can be stored
// then Token Protect mode can be used instead of Seal mode.
//
// Token Protect mode produces authentication tag and other auxiliary data
// (aka ``authentication token'') in a detached buffer.
// This keeps the original size of the encrypted data
// while enabling separate storage of security information.
// Note that the same token must be provided along with the correct secret
// and matching associated context in order for the data to be decrypted successfully.
//
// Since Token Protect mode uses the same security parameters as Seal mode
// (just stored in a different location),
// these modes have the same highest security level.
// Token Protect mode only requires slightly more effort
// in exchange for preserving the original data size.
//
// Read more about Token Protect mode here:
//
// https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#token-protect-mode
type SecureCellTokenProtect struct {
	key *keys.SymmetricKey
}

// TokenProtectWithKey makes a new Secure Cell in Token Protect mode secured by a symmetric key.
func TokenProtectWithKey(key *keys.SymmetricKey) (*SecureCellTokenProtect, error) {
	if key == nil || len(key.Value) == 0 {
		return nil, ErrMissingKey
	}
	return &SecureCellTokenProtect{key}, nil
}

// Encrypt message.
//
// Message is encrypted and authentication token is produced separately.
//
// The context, if provided, is cryptographically mixed with the data,
// but is not included into the resulting encrypted message.
// You will have to provide the same context again during decryption.
// Usually this is some plaintext data associated with encrypted data,
// such as database row number, protocol message ID, etc.
// Empty, and nil contexts are identical.
//
// Encrypted data and authentication token are returned as byte slices.
//
// An error is returned on failure, such as if the message is empty,
// or in case of some internal failure in cryptographic backend.
func (sc *SecureCellTokenProtect) Encrypt(message, context []byte) (encrypted, token []byte, e error) {
	if len(message) == 0 {
		return nil, nil, ErrMissingMessage
	}

	encryptedLength, tokenLength, err := encryptTokenProtect(sc.key.Value, message, context, nil, nil)
	if err != errors.BufferTooSmall {
		return nil, nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	encrypted = make([]byte, encryptedLength)
	token = make([]byte, tokenLength)
	encryptedLength, tokenLength, err = encryptTokenProtect(sc.key.Value, message, context, encrypted, token)
	if err != errors.Success {
		return nil, nil, errors.NewWithCode(err, "Secure Cell failed to encrypt")
	}
	return encrypted[:encryptedLength], token[:tokenLength], nil
}

// Decrypt message.
//
// Secure Cell validates association with the context data, decrypts the message,
// and verifies data integrity using the provided authentication token.
//
// You need to provide the same context as used during encryption.
// (If there was no context you can use empty or nil value).
//
// Non-empty decrypted data is returned if everything goes well.
//
// An error will be returned on failure, such as if the message or token is empty,
// or if the data has been tampered with,
// or if the secret or associated context do not match the ones used for encryption.
func (sc *SecureCellTokenProtect) Decrypt(encrypted, token, context []byte) ([]byte, error) {
	if len(encrypted) == 0 {
		return nil, ErrMissingMessage
	}
	if len(token) == 0 {
		return nil, ErrMissingToken
	}

	length, err := decryptTokenProtect(sc.key.Value, encrypted, token, context, nil)
	if err != errors.BufferTooSmall {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	decrypted := make([]byte, length)
	length, err = decryptTokenProtect(sc.key.Value, encrypted, token, context, decrypted)
	if err != errors.Success {
		return nil, errors.NewWithCode(err, "Secure Cell failed to decrypt")
	}
	return decrypted[:length], nil
}

func encryptTokenProtect(symmetricKey, plaintext, userContext, ciphertext, authToken []byte) (int, int, errors.ThemisErrorCode) {
	authTokenLength := bytesSize(authToken)
	ciphertextLength := bytesSize(ciphertext)
	err := C.themis_secure_cell_encrypt_token_protect(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(plaintext),
		bytesSize(plaintext),
		bytesData(authToken),
		&authTokenLength,
		bytesData(ciphertext),
		&ciphertextLength,
	)
	if sizeOverflow(ciphertextLength) || sizeOverflow(authTokenLength) {
		return 0, 0, errors.NoMemory
	}
	return int(ciphertextLength), int(authTokenLength), errors.ThemisErrorCode(err)
}

func decryptTokenProtect(symmetricKey, ciphertext, authToken, userContext, plaintext []byte) (int, errors.ThemisErrorCode) {
	plaintextLength := bytesSize(plaintext)
	err := C.themis_secure_cell_decrypt_token_protect(
		bytesData(symmetricKey),
		bytesSize(symmetricKey),
		bytesData(userContext),
		bytesSize(userContext),
		bytesData(ciphertext),
		bytesSize(ciphertext),
		bytesData(authToken),
		bytesSize(authToken),
		bytesData(plaintext),
		&plaintextLength,
	)
	if sizeOverflow(plaintextLength) {
		return 0, errors.NoMemory
	}
	return int(plaintextLength), errors.ThemisErrorCode(err)
}
