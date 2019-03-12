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
	"github.com/cossacklabs/themis/gothemis/cell"
	"io"
	"os"
)

func writeByteString(w io.Writer, str []byte) {
	binary.Write(w, binary.BigEndian, uint32(len(str)))
	w.Write(str)
}

func main() {
	args := os.Args

	if (len(args) != 3) && (len(args) != 4) {
		fmt.Printf("usage:\n\t%s <key> <message> [context]\n", args[0])
		os.Exit(1)
	}

	key := []byte(args[1])
	message := []byte(args[2])

	var context []byte

	if len(args) == 4 {
		context = []byte(args[3])
	}

	sc := cell.New(key, cell.ModeSeal)

	encrypted, _, _ := sc.Protect(message, context)

	writeByteString(os.Stdout, key)
	writeByteString(os.Stdout, context)
	writeByteString(os.Stdout, encrypted)
}
