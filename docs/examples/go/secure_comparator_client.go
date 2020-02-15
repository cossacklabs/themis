package main

import (
	"fmt"
	"net"
	"os"

	"github.com/cossacklabs/themis/gothemis/compare"
)

func main() {
	if 2 != len(os.Args) {
		fmt.Printf("usage: %s <password>", os.Args[0])
		os.Exit(1)
	}
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Println("connection error")
		os.Exit(1)
	}
	sc, err := compare.New()
	if err != nil {
		fmt.Println("error creating secure comparator object")
		os.Exit(1)
	}

	err = sc.Append([]byte(os.Args[1]))
	if err != nil {
		fmt.Println("error appending secret to secure comparator")
		os.Exit(1)
	}

	buf, err := sc.Begin()
	if err != nil {
		fmt.Println("error creating init message")
		os.Exit(1)
	}

	for {
		_, err = conn.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			os.Exit(1)
		}

		res, err := sc.Result()
		if err != nil {
			fmt.Println("error geting result from secure comparator")
			os.Exit(1)
		}

		if compare.NotReady == res {
			buf = make([]byte, 10240)
			readBytes, err := conn.Read(buf)
			if err != nil {
				fmt.Println("error reading bytes from socket")
				os.Exit(1)
			}
			buffer, err := sc.Proceed(buf[:readBytes])
			if nil != err {
				fmt.Println("error unwraping message")
				os.Exit(1)
			}
			buf = buffer
		} else {
			if compare.Match == res {
				fmt.Println("match")
				break
			} else {
				fmt.Println("not match")
				os.Exit(1)
			}
		}
	}
}
