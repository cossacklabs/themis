package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
)

func main() {
	keyPair, err := keys.New(keys.TypeEC)
	if nil != err {
		fmt.Println("Keypair generating error")
		return
	}
	fmt.Println(base64.StdEncoding.EncodeToString(keyPair.Private.Value))
	fmt.Println(base64.StdEncoding.EncodeToString(keyPair.Public.Value))
	return
}
