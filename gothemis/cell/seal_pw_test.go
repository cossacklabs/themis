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
	"encoding/binary"
	"fmt"
	"testing"
	"unicode/utf16"

	"github.com/cossacklabs/themis/gothemis/keys"
)

func TestSealPWWithFixedKey(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil || cell == nil {
		t.Error("known passphrase should be allowed", err)
	}
}

func TestSealPWWithBinaryData(t *testing.T) {
	// If you *really* need to use a binary passphrase, cast it to a string.
	// But maybe SealWithKey() will work better for you?
	passphraseBytes, err := base64.StdEncoding.DecodeString(`
H4sICOqUoF4CA3gAjZDLDcAwCENXsfT237FpAMdVe6jygRgwDkhirb3XUXvbvc0OwcSM9KOv8Qt3S
rGUGeoEMEg4p7QYuz+Rj0Iq4kjXtz5LIb9AFv3RF8Nyit8WQep7zU8j68HeDS66EUKSkAEAAA==`)
	if err != nil {
		t.Fatal("cannot decode passphrase bytes", err)
	}
	cell, err := SealWithPassphrase(string(passphraseBytes))
	if err != nil || cell == nil {
		t.Error("binary passphrase should be allowed", err)
	}
}

func TestSealPWWithEmptyPassphrase(t *testing.T) {
	cell, err := SealWithPassphrase("")
	if cell != nil || err != ErrMissingPassphrase {
		t.Error("empty passphrase is not be allowed", err)
	}
}

func ExampleSecureCellSealPassphrase() {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		return
	}

	message := []byte("Would you show me your... seal?")
	context := []byte("Why would you ask for such a thing?")

	encrypted, err := cell.Encrypt(message, context)
	if err != nil {
		return
	}

	decrypted, err := cell.Decrypt(encrypted, context)
	if err != nil {
		return
	}

	fmt.Println(string(decrypted))
	// Output: Would you show me your... seal?
}

func TestSealPWDataLengthExtension(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	if len(encrypted) <= len(message) {
		t.Error("encrypted data must be longer than original")
	}
}

func TestSealPWContextInclusion(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")
	shortContext := []byte("Why would you ask for such a thing?")
	longContext := []byte("But it's so cute! I'm definitely taking it home with me!")

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

func TestSealPWWithoutContext(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

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

func TestSealPWContextSignificance(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")
	contextA := []byte("Why would you ask for such a thing?")
	contextB := []byte("I don't show it to just anyone!")

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

func TestSealPWDetectCorruptedData(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

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

func TestSealPWDetectTruncatedData(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

	encrypted, err := cell.Encrypt(message, nil)
	if err != nil {
		t.Fatal("failed to encrypt message", err)
	}

	_, err = cell.Decrypt(encrypted[:len(encrypted)-1], nil)
	if err == nil {
		t.Error("Secure Cell will detect truncated messages")
	}
}

func TestSealPWDetectExtendedData(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

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

func TestSealPWEmptyMessages(t *testing.T) {
	cell, err := SealWithPassphrase("friendship")
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

func TestSealPWIncompatibleWithKeys(t *testing.T) {
	cellPW, err := SealWithPassphrase("friendship")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}
	cellSK, err := SealWithKey(&keys.SymmetricKey{Value: []byte("friendship")})
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	message := []byte("Would you show me your... seal?")

	encrypted, err := cellPW.Encrypt(message, nil)
	if err != nil {
		t.Error("cannot encrypt message with passphrase", err)
	}
	decrypted, err := cellSK.Decrypt(encrypted, nil)
	if err == nil || bytes.Equal(decrypted, message) {
		t.Error("symmetric key is not a passphrase", err)
	}

	encrypted, err = cellSK.Encrypt(message, nil)
	if err != nil {
		t.Error("cannot encrypt message with symmetric key", err)
	}
	decrypted, err = cellPW.Decrypt(encrypted, nil)
	if err == nil || bytes.Equal(decrypted, message) {
		t.Error("passphrase is not a symmetric key", err)
	}
}

func TestSealPWDefaultEncoding(t *testing.T) {
	cell, err := SealWithPassphrase("дружбомагия")
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	// Message encrypted by PyThemis
	encrypted, _ := base64.StdEncoding.DecodeString(`
AAEBQQwAAAAQAAAAHwAAABYAAABnwwTrIwAUgrvzeuBmgIXw8GaiYqD1yshnGbolQA0DABAA/5SaUH
31a/xYUXiDzRj3W2Lv4hwcbSymrFpbVuIoG7+DDNbo1yh2p2slIwZnMPU=`)
	message := []byte("Would you show me your... seal?")

	decrypted, err := cell.Decrypt(encrypted, nil)
	if err != nil {
		t.Fatal("failed to decrypt message with UTF-8 passphrase", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("message decrypted with UTF-8 passphrase is incorrect")
	}
}

func TestSealPWCustomEncoding(t *testing.T) {
	passphraseUTF16LE := encodeToUTF16("дружбомагия")
	cell, err := SealWithPassphrase(string(passphraseUTF16LE))
	if err != nil {
		t.Fatal("failed to create Secure Cell", err)
	}

	// Message encrypted by PyThemis
	encrypted, _ := base64.StdEncoding.DecodeString(`
AAEBQQwAAAAQAAAAHwAAABYAAABdqqUqnoJqMrmbZeG2Ga11qg8ilWPikSdfraNEQA0DABAAF5hJlG
mZGcjxZUDmq1rMb6jnv8pk+6lc8MpMd84QCWv/WI/LcZ+murJrsZmTAsw=`)
	message := []byte("Would you show me your... seal?")

	decrypted, err := cell.Decrypt(encrypted, nil)
	if err != nil {
		t.Fatal("failed to decrypt message with UTF-16 passphrase", err)
	}
	if !bytes.Equal(decrypted, message) {
		t.Error("message decrypted with UTF-16 passphrase is incorrect")
	}
}

func encodeToUTF16(s string) []byte {
	buffer := new(bytes.Buffer)
	binary.Write(buffer, binary.LittleEndian, utf16.Encode([]rune(s)))
	// Themis is known to fail with byte slices returned by bytes.Buffer
	// so copy it into a new slice, just in case.
	result := make([]byte, buffer.Len())
	copy(result, buffer.Bytes())
	return result
}
