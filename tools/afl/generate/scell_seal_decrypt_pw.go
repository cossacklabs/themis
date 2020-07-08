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

// Test input generator for "scell_seal_decrypt_pw" fuzzer
//
// Run this tool to generate new test input for fuzzer:
//
//     cd tools/afl
//     go run generate/scell_seal_decrypt_pw.go "secret" "message" "context" \
//         > input/scell_seal_decrypt_pw/example.dat
//
// "context" may be omitted. Output is binary so you want to redirect it.
//
// Themis must be installed. It is a good idea to build it with a lower PBKDF2
// iteration count (e.g., THEMIS_DEFAULT_PBKDF2_ITERATIONS=10) to get fuzzing
// results faster.

import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"unsafe"
)

/*
#cgo LDFLAGS: -lthemis

#include <stdbool.h>

#include <themis/themis.h>

static bool themis_scell_encrypt_seal_with_passphrase(
	const void*     passphrase,
	size_t          passphrase_length,
	const void*     user_context,
	size_t          user_context_length,
	const void*     message,
	size_t          message_length,
	void*           encrypted_message,
	size_t*         encrypted_message_length)
{
	themis_status_t res = themis_secure_cell_encrypt_seal_with_passphrase(
		passphrase,
		passphrase_length,
		user_context,
		user_context_length,
		message,
		message_length,
		encrypted_message,
		encrypted_message_length
	);
	return (res == THEMIS_SUCCESS) || (res == THEMIS_BUFFER_TOO_SMALL);
}
*/
import "C"

// TODO: use GoThemis once passphrase API is available there
func encrypt(passphrase, context, message []byte) ([]byte, error) {
	var encryptedSize C.size_t
	if !C.themis_scell_encrypt_seal_with_passphrase(unsafe.Pointer(&passphrase[0]),
		C.size_t(len(passphrase)),
		unsafe.Pointer(&context[0]),
		C.size_t(len(context)),
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		nil,
		&encryptedSize,
	) {
		return nil, fmt.Errorf("failed to compute output size")
	}
	encrypted := make([]byte, encryptedSize)
	if !C.themis_scell_encrypt_seal_with_passphrase(unsafe.Pointer(&passphrase[0]),
		C.size_t(len(passphrase)),
		unsafe.Pointer(&context[0]),
		C.size_t(len(context)),
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		unsafe.Pointer(&encrypted[0]),
		&encryptedSize,
	) {
		return nil, fmt.Errorf("failed to encrypt data")
	}
	return encrypted, nil
}

func writeByteString(w io.Writer, str []byte) {
	binary.Write(w, binary.BigEndian, uint32(len(str)))
	w.Write(str)
}

func main() {
	args := os.Args

	if (len(args) != 3) && (len(args) != 4) {
		fmt.Printf("usage:\n\t%s <passphrase> <message> [<context>]\n", args[0])
		os.Exit(1)
	}

	passphrase := []byte(args[1])
	message := []byte(args[2])

	var context []byte
	if len(args) == 4 {
		context = []byte(args[3])
	}

	encrypted, err := encrypt(passphrase, context, message)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt: %s\n", err)
		os.Exit(1)
	}

	writeByteString(os.Stdout, passphrase)
	writeByteString(os.Stdout, context)
	writeByteString(os.Stdout, encrypted)
}
