package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
)

func main() {
	key_pair, err := keys.New(keys.KEYTYPE_EC)
	if nil != err {
		fmt.Println("Keypair generating error")
		return
	}
	fmt.Println(base64.StdEncoding.EncodeToString(key_pair.Private.Value))
	fmt.Println(base64.StdEncoding.EncodeToString(key_pair.Public.Value))
	return
}
