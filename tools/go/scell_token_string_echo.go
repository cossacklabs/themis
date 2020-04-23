/*
 * Copyright (c) 2017 Cossack Labs Limited
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
	"encoding/base64"
	"fmt"
	"os"
	"strings"

	"github.com/cossacklabs/themis/gothemis/cell"
	"github.com/cossacklabs/themis/gothemis/keys"
)

func main() {
	args := os.Args

	if len(args) < 4 || len(args) > 5 {
		fmt.Printf("usage:\n\t%s {enc|dec} <key> <message[,token]> [context]\n", os.Args[0])
		os.Exit(1)
	}

	command := args[1]
	key := args[2]

	messageAndToken := strings.Split(args[3], ",")
	message := messageAndToken[0]
	var token string
	if len(messageAndToken) == 2 {
		token = messageAndToken[1]
	}

	var context string
	if len(args) == 5 {
		context = args[4]
	}

	sc, err := cell.TokenProtectWithKey(&keys.SymmetricKey{Value: []byte(key)})
	if err != nil {
		fmt.Fprintf(os.Stderr, "Invalid key: \"%s\"", key)
		os.Exit(1)
	}

	switch command {
	case "enc":
		encrypted, token, err := sc.Encrypt([]byte(message), []byte(context))
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error encrypting message")
			os.Exit(1)
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encrypted) + "," + base64.StdEncoding.EncodeToString(token))

	case "dec":
		decodedMessage, err := base64.StdEncoding.DecodeString(message)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decoding message")
			os.Exit(1)
		}
		decodedToken, err := base64.StdEncoding.DecodeString(token)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decoding token")
			os.Exit(1)
		}
		decrypted, err := sc.Decrypt(decodedMessage, decodedToken, []byte(context))
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decrypted))

	default:
		fmt.Fprintln(os.Stderr, "Wrong command, use \"enc\" or \"dec\"")
		os.Exit(1)
	}
}
