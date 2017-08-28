package main

import (
	"fmt"
	"github.com/cossacklabs/themis/gothemis/compare"
	"net"
	"os"
)

func connectionHandler(c net.Conn, secret string) {
	sc, err := compare.New()
	if err != nil {
		fmt.Println("error creating secure comparator object")
		return
	}
	err = sc.Append([]byte(secret))
	if err != nil {
		fmt.Println("error appending secret to secure comparator")
		return
	}
	for {
		res, err := sc.Result()
		if err != nil {
			fmt.Println("error geting result from secure comparator")
			return
		}

		if compare.COMPARE_NOT_READY == res {
			buf := make([]byte, 10240)
			readed_bytes, err := c.Read(buf)
			if err != nil {
				fmt.Println("error reading bytes from socket")
				return
			}
			buf, err = sc.Proceed(buf[:readed_bytes])
			if nil != err {
				fmt.Println("error proceeding message")
				return
			}
			_, err = c.Write(buf)
			if err != nil {
				fmt.Println("error writing bytes from socket")
				return
			}
		} else {
			if compare.COMPARE_MATCH == res {
				fmt.Println("match")
			} else {
				fmt.Println("not match")
			}
			break
		}
	}
}

func main() {
	if 2 != len(os.Args) {
		fmt.Printf("usage: %s <password>", os.Args[0])
		return
	}
	l, err := net.Listen("tcp", ":8080")
	if err != nil {
		fmt.Println("listen error")
		return
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("accepting error")
			return
		}

		go connectionHandler(conn, os.Args[1])
	}
}
