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

func TestSealWithGeneratedKey(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil || cell == nil {
		t.Error("generated key should be allowed", err)
	}
}

func TestSealWithFixedKey(t *testing.T) {
	keyBytes, err := base64.StdEncoding.DecodeString("UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg")
	if err != nil {
		t.Fatal("cannot decode symmetric key", err)
	}
	cell, err := SealWithKey(&keys.SymmetricKey{Value: keyBytes})
	if err != nil || cell == nil {
		t.Error("known key should be allowed", err)
	}
}

func TestSealWithEmptyKey(t *testing.T) {
	cell, err := SealWithKey(nil)
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key should not be allowed", err)
	}
	cell, err = SealWithKey(&keys.SymmetricKey{Value: nil})
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key value should not be allowed", err)
	}
	cell, err = SealWithKey(&keys.SymmetricKey{Value: []byte{}})
	if cell != nil || err != ErrMissingKey {
		t.Error("empty key value should not be allowed", err)
	}
}

func ExampleSecureCellSeal() {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		return
	}

	cell, err := SealWithKey(key)
	if err != nil {
		return
	}

	message := []byte("The corpse. Did you bury it properly?")
	context := []byte("-- 34 to K1 in her car")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		return
	}

	decrypted, err := cell.Decrypt(encrypted, context)
	if err != nil {
		return
	}

	fmt.Println(string(decrypted))
	// Output: The corpse. Did you bury it properly?
}

func TestSealDataLengthExtension(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encrypted) <= len(message) {
		t.Error("encrypted data must be longer than original")
	}
}

func TestSealContextInclusion(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")
	shortContext := []byte("-- 34 to K1 in her car")
	longContext := []byte("If a corpse is buried too shallow, stray dogs might smell it. Dogs like to dig...")

	encryptedShort, err := cell.Encrypt(message, shortContext)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	encryptedLong, err := cell.Encrypt(message, longContext)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encryptedShort) != len(encryptedLong) {
		t.Error("length of encrypted message must not depend on length of context")
	}
}

func TestSealWithoutContext(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")

	// Absent, empty, or nil context are all the same.
	encrypted1, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	encrypted2, err := cell.Encrypt(message, []byte{})
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	var decrypted []byte

	decrypted, err = cell.Decrypt(encrypted1, nil)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
	decrypted, err = cell.Decrypt(encrypted2, nil)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}

	decrypted, err = cell.Decrypt(encrypted1, []byte{})
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
	decrypted, err = cell.Decrypt(encrypted2, []byte{})
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("decrypted result is incorrect")
	}
}

func TestSealContextSignificance(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")
	contextA := []byte("-- 34 to K1 in her car")
	contextB := []byte("We never met this night, okay?")

	encrypted, err := cell.Encrypt(message, contextA)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted, contextB)
	if err == nil {
		t.Error("message should not be decrypted with incorrect context")
	}

	decrypted, err := cell.Decrypt(encrypted, contextA)
	if err != nil {
		t.Error("correct context should allow decryption", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("original message is returned with correct context")
	}
}

func TestSealDetectCorruptedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	// Invert every odd byte, this will surely break the message.
	for i := range encrypted {
		if i%2 == 1 {
			encrypted[i] = encrypted[i] ^ 0xFF
		}
	}

	_, err = cell.Decrypt(encrypted, nil)
	if err == nil {
		t.Error("Secure Cell will detect corrupted messages")
	}
}

func TestSealDetectTruncatedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted[:len(encrypted)-1], nil)
	if err == nil {
		t.Error("Secure Cell will detect truncated messages")
	}
}

func TestSealDetectExtendedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	encrypted = append(encrypted, 0)

	_, err = cell.Decrypt(encrypted, nil)
	if err == nil {
		t.Error("Secure Cell will detect extended messages")
	}
}

func TestSealEmptyMessages(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	_, err = cell.Encrypt(nil, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}
	_, err = cell.Encrypt([]byte{}, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}

	_, err = cell.Decrypt(nil, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}
	_, err = cell.Decrypt([]byte{}, nil)
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}
}

func TestSealCompatibilityWithOldApi(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}

	cellOld := New(key.Value, ModeSeal)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}
	cellNew, err := SealWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("The corpse. Did you bury it properly?")
	context := []byte("-- 34 to K1 in her car")

	encrypted, _, err := cellOld.Protect(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	decrypted, err := cellNew.Decrypt(encrypted, context)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("old API => new API should work fine")
	}

	encrypted, err = cellNew.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}
	decrypted, err = cellOld.Unprotect(encrypted, nil, context)
	if err != nil {
		t.Error("failed to decrypt message", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("new API => old API should work fine")
	}
}
