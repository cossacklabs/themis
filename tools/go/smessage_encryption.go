package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	smessage "github.com/cossacklabs/themis/gothemis/message"
	"io/ioutil"
	"os"
)

func main() {
	args := os.Args

	if len(args) != 5 {
		fmt.Printf("Usage: %s <command: enc | dec | sign | verify > <send_private_key> <recipient_public_key> <message>\n", os.Args[0])
		os.Exit(1)
	}

	command := args[1]
	privateKeyPath := args[2]
	privateKey, err := ioutil.ReadFile(string(privateKeyPath))
	if err != nil {
		panic(err)
	}
	publicKeyPath := args[3]
	publicKey, err := ioutil.ReadFile(string(publicKeyPath))
	if err != nil {
		panic(err)
	}
	message := args[4]

	messageEncrypter := smessage.New(&keys.PrivateKey{Value: privateKey}, &keys.PublicKey{Value: publicKey})

	if "enc" == command {
		encData, err := messageEncrypter.Wrap([]byte(message))
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error encrypting message")
			os.Exit(1)
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encData))

	} else if "dec" == command {
		decodedMessage, err := base64.StdEncoding.DecodeString(message)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decoding message")
			os.Exit(1)
		}

		decData, err := messageEncrypter.Unwrap(decodedMessage)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decData[:]))

	} else if "sign" == command {
		encData, err := messageEncrypter.Sign([]byte(message))
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error encrypting message")
			os.Exit(1)
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encData))
	} else if "verify" == command {
		decodedMessage, err := base64.StdEncoding.DecodeString(message)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decoding message")
			os.Exit(1)
		}

		decData, err := messageEncrypter.Verify(decodedMessage)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decData[:]))

	} else {
		fmt.Fprintln(os.Stderr, "Wrong command, use <enc | dev | sign | verify>")
		os.Exit(1)
	}
}
