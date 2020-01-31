// Copyright (c) 2020 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package main

// Test vector generator for Soter KDF (soter_kdf())
//
// Run this tool directly:
//
//     go run tests/soter/test_vectors/soter_kdf.go
//
// Test vectors are computed and printed on stdout.
// Paste them into tests/soter/soter_kdf_test.c

import (
	"crypto/hmac"
	"crypto/sha256"
	"fmt"
	"strings"
)

var key = []byte{0xAD, 0xEA, 0xDB, 0xEE, 0x01, 0x02, 0x03, 0x04}
var label = "ZRTP KDF test vectors"
var context = []byte{0x43, 0x83, 0xF3, 0x27, 0x0E, 0x11}
var lengths = []int{1, 2, 4, 8, 16, 32}

// A la RFC 6189 with HMAC-SHA-256
// KDF(KI, label, context, L) = HMAC(KI, i || label || 0x00 || context || L)
func zrtpKDF(key []byte, label string, context []byte, length int) []byte {
	if !(1 <= length && length <= 32) {
		panic("invalid length")
	}
	data := make([]byte, 0, len(label)+len(context)+5)
	data = append(data, []byte{0x00, 0x00, 0x00, 0x01}...)
	data = append(data, []byte(label)...)
	data = append(data, 0x00)
	data = append(data, context...)
	// Here ZRTP KDF appends "length", but Soter KDF omits it
	mac := hmac.New(sha256.New, key)
	mac.Write(data)
	res := mac.Sum(nil)
	return res[:length]
}

func hexString(data []byte) string {
	var str strings.Builder
	for _, byte := range data {
		str.WriteString(fmt.Sprintf("\\x%02X", byte))
	}
	return str.String()
}

func main() {
	fmt.Printf("static const uint8_t zrtp_key[] = \"%s\";\n", hexString(key))
	fmt.Printf("static const size_t zrtp_key_length = %d;\n", len(key))
	fmt.Printf("static const char* zrtp_label = \"%s\";\n", label)
	fmt.Printf("static const uint8_t zrtp_context[] = \"%s\";\n", hexString(context))
	fmt.Printf("static const size_t zrtp_context_length = %d;\n", len(context))
	fmt.Printf(`
struct zrtp_kdf_test_vector {
    size_t output_length;
    const void* output;
};
`)
	fmt.Println()
	fmt.Printf("static const struct zrtp_kdf_test_vector zrtp_kdf_test_vectors[] = {\n")
	for _, length := range lengths {
		output := zrtpKDF(key, label, context, length)
		if len(output) != length {
			panic("mismatching length")
		}
		fmt.Printf("    {%d, \"%s\"},\n", length, hexString(output))
	}
	fmt.Printf("};\n")
}
