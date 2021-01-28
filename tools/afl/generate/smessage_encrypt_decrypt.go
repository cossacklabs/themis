/*
 * Copyright (c) 2019 Cossack Labs Limited
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
	"encoding/binary"
	"fmt"
	"io"
	"os"

	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/message"
)

func writeByteString(w io.Writer, str []byte) {
	binary.Write(w, binary.BigEndian, uint32(len(str)))
	w.Write(str)
}

func main() {
	args := os.Args

	if len(args) != 3 {
		fmt.Printf("usage:\n\t%s {EC|RSA} <message>\n", args[0])
		os.Exit(1)
	}

	keyType := -1
	switch args[1] {
	case "ec", "EC":
		keyType = keys.TypeEC
	case "rsa", "RSA":
		keyType = keys.TypeRSA
	default:
		fmt.Fprintf(os.Stderr, "unknown key type: %s\nsupported: EC, RSA\n", args[1])
		os.Exit(1)
	}
	keypair, err := keys.New(keyType)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to generate new keypair: %v\n", err)
		os.Exit(1)
	}

	msg := []byte(args[2])

	sm := message.New(keypair.Private, keypair.Public)

	encrypted, err := sm.Wrap(msg)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to encrypt Secure Message: %v\n", err)
		os.Exit(1)
	}

	writeByteString(os.Stdout, keypair.Private.Value)
	writeByteString(os.Stdout, keypair.Public.Value)
	writeByteString(os.Stdout, encrypted)
}
