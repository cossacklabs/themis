package main

import (
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"io/ioutil"
	"os"
)

func main() {
	args_count := len(os.Args)

	if args_count != 1 && args_count != 3 {
		fmt.Printf("Usage: go %s <private_key_path> <public_key_path>\n", os.Args[0])
		os.Exit(1)
	}

	var private_key_path, public_key_path string
	if args_count == 1 {
		private_key_path = "key"
		public_key_path = "key.pub"
	} else if args_count == 3 {
		private_key_path = os.Args[1]
		public_key_path = os.Args[2]
	}

	keypair, err := keys.New(keys.TypeEC)
	if err != nil {
		panic(err)
	}
	err = ioutil.WriteFile(private_key_path, keypair.Private.Value, 0400)
	if err != nil {
		panic(err)
	}
	err = ioutil.WriteFile(public_key_path, keypair.Public.Value, 0666)
	if err != nil {
		panic(err)
	}
}
