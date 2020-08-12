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

func TestContextImprintWithGeneratedKey(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil || cell == nil {
		t.Error("generated key should be allowed", err)
	}
}

func TestContextImprintWithFixedKey(t *testing.T) {
	keyBytes, err := base64.StdEncoding.DecodeString("UkVDMgAAAC13PCVZAKOczZXUpvkhsC+xvwWnv3CLmlG0Wzy8ZBMnT+2yx/dg")
	if err != nil {
		t.Fatal("cannot decode symmetric key", err)
	}
	cell, err := ContextImprintWithKey(&keys.SymmetricKey{Value: keyBytes})
	if err != nil || cell == nil {
		t.Error("known key should be allowed", err)
	}
}

func TestContextImprintWithEmptyKey(t *testing.T) {
	cell, err := ContextImprintWithKey(nil)
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key should not be allowed", err)
	}
	cell, err = ContextImprintWithKey(&keys.SymmetricKey{Value: nil})
	if cell != nil || err != ErrMissingKey {
		t.Error("nil key value should not be allowed", err)
	}
	cell, err = ContextImprintWithKey(&keys.SymmetricKey{Value: []byte{}})
	if cell != nil || err != ErrMissingKey {
		t.Error("empty key value should not be allowed", err)
	}
}

func ExampleSecureCellContextImprint() {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		return
	}

	cell, err := ContextImprintWithKey(key)
	if err != nil {
		return
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		return
	}

	decrypted, err := cell.Decrypt(encrypted, context)
	if err != nil {
		return
	}

	fmt.Println(string(decrypted))
	// Output: I was... Um, I was wondering why you're carrying the same bat.
}

func TestContextImprintDataLengthPreservation(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encrypted) != len(message) {
		t.Error("encrypted data must have the same length as original")
	}
}

func TestContextImprintContextInclusion(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	shortContext := []byte("Even though he joined a team, he never played any sports.")
	longContext := []byte("And just like you, he also started walking alone one day... Alone just like you... And then one day, suddenly, he started practicing swinging his bat... just like you! And then, one day, he suddenly started carrying a baseball bat around all the time... just like you! And then... one day...")

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

func TestContextImprintContextSignificance(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	contextA := []byte("Even though he joined a team, he never played any sports.")
	contextB := []byte("...One day, suddenly what?")

	encrypted, err := cell.Encrypt(message, contextA)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	// You can use a different context to decrypt data, but you'll get garbage.
	decrypted, err := cell.Decrypt(encrypted, contextB)
	if err != nil || bytes.Equal(decrypted, message) {
		t.Error("message is successfully decrypted into garbage")
	}

	// Only the original context will work.
	decrypted, err = cell.Decrypt(encrypted, contextA)
	if err != nil {
		t.Error("correct context should allow decryption", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("original message is returned with correct context")
	}
}

func TestContextImprintNoDetectCorruptedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	// Invert every odd byte, this will surely break the message.
	for i := range encrypted {
		if i%2 == 1 {
			encrypted[i] = encrypted[i] ^ 0xFF
		}
	}

	// Decrypts successfully but the content is garbage.
	decrypted, err := cell.Decrypt(encrypted, context)
	if err != nil || bytes.Equal(decrypted, message) {
		t.Error("corrupted message is successfully decrypted into garbage")
	}
}

func TestContextImprintNoDetectTruncatedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	decrypted, err := cell.Decrypt(encrypted[:len(encrypted)-1], context)
	if err != nil || bytes.Equal(decrypted, message) {
		t.Error("truncated message is successfully decrypted into garbage")
	}
}

func TestContextImprintNoDetectExtendedData(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	encrypted = append(encrypted, 0)

	decrypted, err := cell.Decrypt(encrypted, context)
	if err != nil || bytes.Equal(decrypted, message) {
		t.Error("extended message is successfully decrypted into garbage")
	}
}

func TestContextImprintMessageAndContextRequired(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}
	cell, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	_, err = cell.Encrypt(nil, []byte("context"))
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}
	_, err = cell.Encrypt([]byte{}, []byte("context"))
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be encrypted", err)
	}

	_, err = cell.Encrypt([]byte("message"), nil)
	if err != ErrMissingContext {
		t.Error("empty context cannot be used for encryption", err)
	}
	_, err = cell.Encrypt([]byte("message"), []byte{})
	if err != ErrMissingContext {
		t.Error("empty context cannot be used for encryption", err)
	}

	_, err = cell.Decrypt(nil, []byte("context"))
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}
	_, err = cell.Decrypt([]byte{}, []byte("context"))
	if err != ErrMissingMessage {
		t.Error("empty messages cannot be decrypted", err)
	}

	_, err = cell.Decrypt([]byte("encrypted"), nil)
	if err != ErrMissingContext {
		t.Error("empty context cannot be used for decryption", err)
	}
	_, err = cell.Decrypt([]byte("encrypted"), []byte{})
	if err != ErrMissingContext {
		t.Error("empty context cannot be used for decryption", err)
	}
}

func TestContextImprintCompatibilityWithOldApi(t *testing.T) {
	key, err := keys.NewSymmetricKey()
	if err != nil {
		t.Fatal("cannot generate symmetric key", err)
	}

	cellOld := New(key.Value, ModeContextImprint)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}
	cellNew, err := ContextImprintWithKey(key)
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("I was... Um, I was wondering why you're carrying the same bat.")
	context := []byte("Even though he joined a team, he never played any sports.")

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
