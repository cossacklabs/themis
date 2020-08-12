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

import (
	"bytes"
	"encoding/base64"
	"fmt"
	"testing"

	"github.com/cossacklabs/themis/gothemis/keys"
)

func TestTokenProtectWithGeneratedKey(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil || cell == nil {
		t.Error("generated key should be allowed", err)
	}
}

func TestTokenProtectWithFixedKey(t *testing.T) {
	keyBytes, err := base64.StdEncoding.DecodeString("UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg")
	if err != nil {
		t.Fatal("cannot decode symmetric key", err)
	}
	cell, err := TokenProtectWithKey(&keys.SymmetricKey{Value: keyBytes})
	if err != nil || cell == nil {
		t.Error("known key should be allowed", err)
	}
}

func TestTokenProtectWithEmptyKey(t *testing.T) {
	cell, err := TokenProtectWithKey(nil)
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key should not be allowed", err)
	}
	cell, err = TokenProtectWithKey(&keys.SymmetricKey{Value: nil})
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key value should not be allowed", err)
	}
	cell, err = TokenProtectWithKey(&keys.SymmetricKey{Value: []byte{}})
	if cell != nil || err != ErrMissingKey {
		t.Error("empty key value should not be allowed", err)
	}
}

func ExampleSecureCellTokenProtect() {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		return
	}

	cell, err := TokenProtectWithKey(key)
	if err != nil {
		return
	}

	message := []byte("Should we speak in my car?")
	context := []byte("I have AC running there")

	encrypted, token, err := cell.Encrypt(message, context)
	if err != nil {
		return
	}

	decrypted, err := cell.Decrypt(encrypted, token, context)
	if err != nil {
		return
	}

	fmt.Println(string(decrypted))
	// Output: Should we speak in my car?
}

func TestTokenProtectDataLengthPreserved(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encrypted) != len(message) {
		t.Error("encrypted data must be equal to original")
	}
	if len(token) == 0 {
		t.Error("token must not be empty")
	}
}

func TestTokenProtectContextInclusion(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")
	shortContext := []byte("I have AC running there")
	longContext := []byte("I really hate hot weather, you know?")

	encryptedShort, tokenShort, err := cell.Encrypt(message, shortContext)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	encryptedLong, tokenLong, err := cell.Encrypt(message, longContext)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encryptedShort) != len(encryptedLong) {
		t.Error("length of encrypted message must not depend on length of context")
	}
	if len(tokenShort) != len(tokenLong) {
		t.Error("length of authentication token must not depend on length of context")
	}
}

func TestTokenProtectWithoutContext(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	// Absent, empty, or nil context are all the same.
	encrypted1, token1, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	encrypted2, token2, err := cell.Encrypt(message, []byte{})
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	var decrypted []byte

	decrypted, err = cell.Decrypt(encrypted1, token1, nil)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
	decrypted, err = cell.Decrypt(encrypted2, token2, nil)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}

	decrypted, err = cell.Decrypt(encrypted1, token1, []byte{})
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
	decrypted, err = cell.Decrypt(encrypted2, token2, []byte{})
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
}

func TestTokenProtectContextSignificance(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")
	contextA := []byte("I have AC running there")
	contextB := []byte("I really hate hot weather, you know?")

	encrypted, token, err := cell.Encrypt(message, contextA)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted, token, contextB)
	if err == nil {
		t.Error("message should not be decrypted with incorrect context")
	}

	decrypted, err := cell.Decrypt(encrypted, token, contextA)
	if err != nil {
		t.Error("correct context should allow decryption", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("original message is returned with correct context")
	}
}

func TestTokenProtectTokenSignificance(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted1, token1, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	encrypted2, token2, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted1, token2, nil)
	if err == nil {
		t.Error("message should not be decrypted with incorrect token")
	}
	_, err = cell.Decrypt(encrypted1, token2, nil)
	if err == nil {
		t.Error("message should not be decrypted with incorrect token")
	}

	decrypted, err := cell.Decrypt(encrypted1, token1, nil)
	if err != nil {
		t.Error("correct token should allow decryption", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("original message is returned with correct token")
	}
	decrypted, err = cell.Decrypt(encrypted2, token2, nil)
	if err != nil {
		t.Error("correct token should allow decryption", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("original message is returned with correct token")
	}
}

func TestTokenProtectDetectCorruptedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	// Invert every odd byte, this will surely break the message.
	for i := range encrypted {
		if i%2 == 1 {
			encrypted[i] = encrypted[i] ^ 0xFF
		}
	}

	_, err = cell.Decrypt(encrypted, token, nil)
	if err == nil {
		t.Error("Secure Cell will detect corrupted messages")
	}
}

func TestTokenProtectDetectTruncatedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted[:len(encrypted)-1], token, nil)
	if err == nil {
		t.Error("Secure Cell will detect truncated messages")
	}
}

func TestTokenProtectDetectExtendedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	encrypted = append(encrypted, 0)

	_, err = cell.Decrypt(encrypted, token, nil)
	if err == nil {
		t.Error("Secure Cell will detect extended messages")
	}
}

func TestTokenProtectDetectCorruptedToken(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	// Invert every odd byte, this will surely break the token.
	for i := range token {
		if i%2 == 1 {
			token[i] = token[i] ^ 0xFF
		}
	}

	_, err = cell.Decrypt(encrypted, token, nil)
	if err == nil {
		t.Error("Secure Cell will detect corrupted tokens")
	}
}

func TestTokenProtectDetectTruncatedToken(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted, token[:len(token)-1], nil)
	if err == nil {
		t.Error("Secure Cell will detect truncated tokens")
	}
}

func TestTokenProtectDetectExtendedToken(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	token = append(token, 0)

	// Current implementation of Secure Cell allows the token to be overlong.
	// Extra data is simply ignored.
	decrypted, err := cell.Decrypt(encrypted, token, nil)
	if err != nil {
		t.Error("Secure Cell does not detect extended tokens")
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("Secure Cell decrypts messages with extended tokens")
	}
}

func TestTokenProtectSwapDataAndToken(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")

	encrypted, token, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(token, encrypted, nil)
	if err == nil {
		t.Error("Secure Cell will detect if token and message are swappeds")
	}
}

func TestTokenProtectEmptyMessages(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	_, _, err = cell.Encrypt(nil, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}
	_, _, err = cell.Encrypt([]byte{}, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}

	encrypted, token, err := cell.Encrypt([]byte("."), nil)
	if err != nil {
		t.Fatal("cannot encrypt message", err)
	}

	_, err = cell.Decrypt(nil, token, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}
	_, err = cell.Decrypt([]byte{}, token, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}
	_, err = cell.Decrypt(encrypted, nil, nil)
	if err != ErrMissingToken {
		t.Error("empty messages cannot be decrypted", err)
	}
	_, err = cell.Decrypt(encrypted, []byte{}, nil)
	if err != ErrMissingToken {
		t.Error("empty messages cannot be decrypted", err)
	}
}

func TestTokenProtectCompatibilityWithOldApi(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}

	cellOld := New(key.Value, ModeTokenProtect)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}
	cellNew, err := TokenProtectWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Should we speak in my car?")
	context := []byte("I have AC running there")

	encrypted, token, err := cellOld.Protect(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	decrypted, err := cellNew.Decrypt(encrypted, token, context)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("old API => new API should work fine")
	}

	encrypted, token, err = cellNew.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	decrypted, err = cellOld.Unprotect(encrypted, token, context)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("new API => old API should work fine")
	}
}
