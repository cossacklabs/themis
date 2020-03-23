package main

import (
	"fmt"
	"net"
	"os"

	"github.com/cossacklabs/themis/gothemis/compare"
)

func connectionHandler(c net.Conn, secret string) {
	sc, err := compare.New()
	if err != nil {
		fmt.Println("error creating secure comparator object")
		os.Exit(1)
	}
	err = sc.Append([]byte(secret))
	if err != nil {
		fmt.Println("error appending secret to secure comparator")
		os.Exit(1)
	}
	for {
		res, err := sc.Result()
		if err != nil {
			fmt.Println("error geting result from secure comparator")
			os.Exit(1)
		}

		if compare.NotReady == res {
			buf := make([]byte, 10240)
			readBytes, err := c.Read(buf)
			if err != nil {
				fmt.Println("error reading bytes from socket")
				os.Exit(1)
			}
			buf, err = sc.Proceed(buf[:readBytes])
			if nil != err {
				fmt.Println("error proceeding message")
				os.Exit(1)
			}
			_, err = c.Write(buf)
			if err != nil {
				fmt.Println("error writing bytes from socket")
				os.Exit(1)
			}
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

func main() {
	if 2 != len(os.Args) {
		fmt.Printf("usage: %s <password>", os.Args[0])
		os.Exit(1)
	}
	l, err := net.Listen("tcp", ":8080")
	if err != nil {
		fmt.Println("listen error")
		os.Exit(1)
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("accepting error")
			os.Exit(1)
		}

		go connectionHandler(conn, os.Args[1])
	}
}
