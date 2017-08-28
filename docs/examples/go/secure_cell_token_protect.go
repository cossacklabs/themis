package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/cell"
	"os"
)

func main() {
	if 4 > len(os.Args) {
		fmt.Printf("usage %s <command> <password> <message> [<token>]\n", os.Args[0])
		return
	}
	sc := cell.New([]byte(os.Args[2]), cell.CELL_MODE_TOKEN_PROTECT)
	if "enc" == os.Args[1] {
		encData, token, err := sc.Protect([]byte(os.Args[3]), nil)
		if nil != err {
			fmt.Println("error encrypting message")
			return
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encData))
		fmt.Println(base64.StdEncoding.EncodeToString(token))
	} else if "dec" == os.Args[1] {
		if 5 != len(os.Args) {
			fmt.Println("token not set")
			return
		}
		decoded_message, err := base64.StdEncoding.DecodeString(os.Args[3])
		if nil != err {
			fmt.Println("error decoding message")
			return
		}
		decoded_token, err := base64.StdEncoding.DecodeString(os.Args[4])
		if nil != err {
			fmt.Println("error decoding token")
			return
		}
		decData, err := sc.Unprotect(decoded_message, decoded_token, nil)
		if nil != err {
			fmt.Println("error decrypting message")
			return
		}
		fmt.Println(string(decData[:]))
	}
}
