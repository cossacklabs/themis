package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/message"
	"os"
)

func main() {
	if 5 != len(os.Args) {
		fmt.Printf("usage: %s <command> <private key> <peer public key> <message>\n", os.Args[0])
		return
	}
	decoded_key, err := base64.StdEncoding.DecodeString(os.Args[2])
	if nil != err {
		fmt.Println("error decoding private key")
		return
	}
	pr := keys.PrivateKey{decoded_key}
	decoded_key, err = base64.StdEncoding.DecodeString(os.Args[3])
	if nil != err {
		fmt.Println("error decoding private key")
		return
	}
	pu := keys.PublicKey{decoded_key}
	sm := message.New(&pr, &pu)
	if "enc" == os.Args[1] {
		wrapped, err := sm.Wrap([]byte(os.Args[4]))
		if nil != err {
			fmt.Println("error encrypting message")
			return
		}
		fmt.Println(base64.StdEncoding.EncodeToString(wrapped))
	} else if "dec" == os.Args[1] {
		decoded_message, err := base64.StdEncoding.DecodeString(os.Args[4])
		if nil != err {
			fmt.Println("error decoding message")
			return
		}
		unwrapped, err := sm.Unwrap(decoded_message)
		if nil != err {
			fmt.Println("error decrypting message")
			return
		}
		fmt.Println(string(unwrapped[:]))

	}
}
