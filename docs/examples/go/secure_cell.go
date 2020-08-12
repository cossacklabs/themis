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

package main

import (
	"bytes"
	"encoding/base64"
	"fmt"
	"os"

	"github.com/cossacklabs/themis/gothemis/cell"
	"github.com/cossacklabs/themis/gothemis/keys"
)

func exampleSeal(key *keys.SymmetricKey, message, context []byte) {
	fmt.Println("Secure Cell - Seal mode (symmetric key)")
	// This is the easiest mode to use.

	scellMK, err := cell.SealWithKey(key)
	if err != nil {
		fmt.Fprintf(os.Stderr, "cannot construct Secure Cell: %v\n", err)
		os.Exit(1)
	}
	encrypted, err := scellMK.Encrypt(message, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt message: %v\n", err)
		os.Exit(1)
	}
	decrypted, err := scellMK.Decrypt(encrypted, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to decrypt message: %v\n", err)
		os.Exit(1)
	}
	if !bytes.Equal(decrypted, message) {
		fmt.Fprintf(os.Stderr, "decrypted content does not match: %s\n", string(decrypted))
		os.Exit(1)
	}
	fmt.Printf("Encoded:   %s\n", base64.StdEncoding.EncodeToString([]byte(message)))
	fmt.Printf("Encrypted: %s\n", base64.StdEncoding.EncodeToString(encrypted))
	fmt.Printf("Decrypted: %s\n", string(decrypted))
	fmt.Printf("\n")
}

func exampleSealWithPassphrase(passphrase string, message, context []byte) {
	fmt.Println("Secure Cell - Seal mode (passphrase)")
	// This is the easiest mode to use if you need to keep the secret in your head.

	scellPW, err := cell.SealWithPassphrase(passphrase)
	if err != nil {
		fmt.Fprintf(os.Stderr, "cannot construct Secure Cell: %v\n", err)
		os.Exit(1)
	}
	encrypted, err := scellPW.Encrypt(message, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt message: %v\n", err)
		os.Exit(1)
	}
	decrypted, err := scellPW.Decrypt(encrypted, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to decrypt message: %v\n", err)
		os.Exit(1)
	}
	if !bytes.Equal(decrypted, message) {
		fmt.Fprintf(os.Stderr, "decrypted content does not match: %s\n", string(decrypted))
		os.Exit(1)
	}
	fmt.Printf("Encoded:   %s\n", base64.StdEncoding.EncodeToString([]byte(message)))
	fmt.Printf("Encrypted: %s\n", base64.StdEncoding.EncodeToString(encrypted))
	fmt.Printf("Decrypted: %s\n", string(decrypted))
	fmt.Printf("\n")
}

func exampleTokenProtect(key *keys.SymmetricKey, message, context []byte) {
	fmt.Println("Secure Cell - Token Protect mode")
	// A bit harded to use than Seal mode due to extra "authentication token",
	// but the encrypted data has the same size as input.

	scellTP, err := cell.TokenProtectWithKey(key)
	if err != nil {
		fmt.Fprintf(os.Stderr, "cannot construct Secure Cell: %v\n", err)
		os.Exit(1)
	}
	encrypted, token, err := scellTP.Encrypt(message, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt message: %v\n", err)
		os.Exit(1)
	}
	decrypted, err := scellTP.Decrypt(encrypted, token, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to decrypt message: %v\n", err)
		os.Exit(1)
	}
	if !bytes.Equal(decrypted, message) {
		fmt.Fprintf(os.Stderr, "decrypted content does not match: %s\n", string(decrypted))
		os.Exit(1)
	}
	fmt.Printf("Encoded:   %s\n", base64.StdEncoding.EncodeToString([]byte(message)))
	fmt.Printf("Encrypted: %s\n", base64.StdEncoding.EncodeToString(encrypted))
	fmt.Printf("Token:     %s\n", base64.StdEncoding.EncodeToString(token))
	fmt.Printf("Decrypted: %s\n", string(decrypted))
	fmt.Printf("\n")
}

func exampleTokenImprint(key *keys.SymmetricKey, message, context []byte) {
	fmt.Println("Secure Cell - Context Imprint mode")
	// Slightly less secure mode which preserves the input length too.
	// However, it requires "associated context" to be specified
	// and does not verify data integrity during decryption.

	scellCI, err := cell.ContextImprintWithKey(key)
	if err != nil {
		fmt.Fprintf(os.Stderr, "cannot construct Secure Cell: %v\n", err)
		os.Exit(1)
	}
	encrypted, err := scellCI.Encrypt(message, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt message: %v\n", err)
		os.Exit(1)
	}
	decrypted, err := scellCI.Decrypt(encrypted, context)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to decrypt message: %v\n", err)
		os.Exit(1)
	}
	if !bytes.Equal(decrypted, message) {
		fmt.Fprintf(os.Stderr, "decrypted content does not match: %s\n", string(decrypted))
		os.Exit(1)
	}
	fmt.Printf("Encoded:   %s\n", base64.StdEncoding.EncodeToString([]byte(message)))
	fmt.Printf("Encrypted: %s\n", base64.StdEncoding.EncodeToString(encrypted))
	fmt.Printf("Decrypted: %s\n", string(decrypted))
	fmt.Printf("\n")
}

func main() {
	// Cryptographic parameters. Keep them secret.
	passphrase := "broccoli"
	key, err := keys.NewSymmetricKey()
	if err != nil {
		fmt.Fprintf(os.Stderr, "cannot generate symmetric key: %v\n", err)
		os.Exit(1)
	}

	// Input data. Note that you always encrypt bytes so strings need to be encoded.
	// Here we just cast them into []byte, effectively encoding in UTF-8.
	message := []byte("Broccoli or cauliflower, which one is green?")
	context := []byte("Correct! *gunshot*")

	exampleSeal(key, message, context)
	exampleSealWithPassphrase(passphrase, message, context)
	exampleTokenProtect(key, message, context)
	exampleTokenImprint(key, message, context)
}
