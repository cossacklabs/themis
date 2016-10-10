package main

import (
    "github.com/cossacklabs/themis/gothemis/keys";
    "fmt";
    "encoding/base64"
)


func main(){
    key_pair, err := keys.New(keys.KEYTYPE_EC)
    if nil != err {
	fmt.Println("Keypair generating error")
	return
    }
    fmt.Println(base64.StdEncoding.EncodeToString(key_pair.Private.Value))
    fmt.Println(base64.StdEncoding.EncodeToString(key_pair.Public.Value))
    return
}

