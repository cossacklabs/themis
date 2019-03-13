package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/cell"
	"os"
)

func main() {
	args := os.Args

	if len(args) != 5 {
		fmt.Printf("usage %s <command: enc | dec > <key> <message> <context>\n", os.Args[0])
		os.Exit(1)
	}

	command := args[1]
	key := args[2]
	message := args[3]
	context := []byte(args[4])

	sc := cell.New([]byte(key), cell.ModeContextImprint)

	if "enc" == command {
		encData, _, err := sc.Protect([]byte(message), context)
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
		decData, err := sc.Unprotect(decodedMessage, nil, context)
		if nil != err {
			fmt.Fprintln(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decData[:]))

	} else {
		fmt.Fprintln(os.Stderr, "Wrong command, use \"enc\" or \"dec\"")
		os.Exit(1)
	}
}
