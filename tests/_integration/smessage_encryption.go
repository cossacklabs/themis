package main

import (
	"encoding/base64"
	"fmt"
	smessage "github.com/cossacklabs/themis/gothemis/message"
	"os"
	"io/ioutil"
	"github.com/cossacklabs/themis/gothemis/keys"
)

func main() {
	args := os.Args

	if len(args) != 5 {
		fmt.Printf("Usage: %s <command: enc | dec | sign | verify > <send_private_key> <recipient_public_key> <message>\n", os.Args[0])
		os.Exit(1)
	}

	command := args[1]
	private_key_path := args[2]
	private_key, err := ioutil.ReadFile(string(private_key_path))
	if err != nil {
		panic(err)
	}
	public_key_path := args[3]
	public_key, err := ioutil.ReadFile(string(public_key_path))
	if err != nil {
		panic(err)
	}
	message := args[4]

	message_encrypter := smessage.New(&keys.PrivateKey{Value: private_key}, &keys.PublicKey{Value: public_key})

	if "enc" == command {
		encData, err := message_encrypter.Wrap([]byte(message))
		if nil != err {
			fmt.Println(os.Stderr, "Error encrypting message")
			os.Exit(1)
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encData))

	} else if "dec" == command {
		decoded_message, err := base64.StdEncoding.DecodeString(message)
		if nil != err {
			fmt.Println(os.Stderr, "Error decoding message")
			os.Exit(1)
		}

		decData, err := message_encrypter.Unwrap(decoded_message)
		if nil != err {
			fmt.Println(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decData[:]))

	} else if "sign" == command {
        encData, err := message_encrypter.Sign([]byte(message))
        if nil != err {
            fmt.Println(os.Stderr, "Error encrypting message")
            os.Exit(1)
        }
        fmt.Println(base64.StdEncoding.EncodeToString(encData))
    } else if "verify" == command {
        decoded_message, err := base64.StdEncoding.DecodeString(message)
        if nil != err {
            fmt.Println(os.Stderr, "Error decoding message")
            os.Exit(1)
        }

        decData, err := message_encrypter.Verify(decoded_message)
        if nil != err {
            fmt.Println(os.Stderr, "Error decrypting message")
            os.Exit(1)
        }
        fmt.Println(string(decData[:]))

    } else {
		fmt.Println(os.Stderr, "Wrong command, use <enc | dev | sign | verify>")
		os.Exit(1)
	}
}
