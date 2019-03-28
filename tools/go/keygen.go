package main

import (
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"io/ioutil"
	"os"
)

func main() {
	argsCount := len(os.Args)

	if argsCount != 1 && argsCount != 3 {
		fmt.Printf("Usage: go %s <private_key_path> <public_key_path>\n", os.Args[0])
		os.Exit(1)
	}

	var privateKeyPath, publicKeyPath string
	if argsCount == 1 {
		privateKeyPath = "key"
		publicKeyPath = "key.pub"
	} else if argsCount == 3 {
		privateKeyPath = os.Args[1]
		publicKeyPath = os.Args[2]
	}

	keypair, err := keys.New(keys.TypeEC)
	if err != nil {
		panic(err)
	}
	err = ioutil.WriteFile(privateKeyPath, keypair.Private.Value, 0400)
	if err != nil {
		panic(err)
	}
	err = ioutil.WriteFile(publicKeyPath, keypair.Public.Value, 0666)
	if err != nil {
		panic(err)
	}
}
