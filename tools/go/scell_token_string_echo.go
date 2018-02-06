package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/cell"
	"os"
	"strings"
)

func main() {
	args := os.Args

	if len(args) < 4 || len(args) > 5 {
		fmt.Printf("usage %s <command: enc | dec > <key> <message,token> <context (optional)>\n", os.Args[0])
		os.Exit(1)
	}

	command := args[1]
	key := args[2]

	message_and_token := strings.Split(args[3], ",")
	message := message_and_token[0]

	var token string
	if len(message_and_token) == 2 {
		token = message_and_token[1] 
	}

	var context []byte

	if len(args) == 5 {
		context = []byte(args[4])
	}

	sc := cell.New([]byte(key), cell.CELL_MODE_TOKEN_PROTECT)

	if "enc" == command {
		encData, encToken, err := sc.Protect([]byte(message), context)
		if nil != err {
			fmt.Println(os.Stderr, "Error encrypting message")
			os.Exit(1)
		}
		fmt.Println(base64.StdEncoding.EncodeToString(encData) + "," + base64.StdEncoding.EncodeToString(encToken))

	} else if "dec" == command {
		decoded_message, err := base64.StdEncoding.DecodeString(message)
		if nil != err {
			fmt.Println(os.Stderr, "Error decoding message")
			os.Exit(1)
		}
		decoded_token, err := base64.StdEncoding.DecodeString(token)
		if nil != err {
			fmt.Println("Error decoding token")
			os.Exit(1)
		}
		decData, err := sc.Unprotect(decoded_message, decoded_token, context)
		if nil != err {
			fmt.Println(os.Stderr, "Error decrypting message")
			os.Exit(1)
		}
		fmt.Println(string(decData[:]))

	} else {
		fmt.Println(os.Stderr, "Wrong command, use \"enc\" or \"dec\"")
		os.Exit(1)
	}
}
