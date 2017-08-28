package main

import (
	"fmt"
	"github.com/cossacklabs/themis/gothemis/compare"
	"net"
	"os"
)

func main() {
	if 2 != len(os.Args) {
		fmt.Printf("usage: %s <password>", os.Args[0])
		return
	}
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Println("connection error")
		return
	}
	sc, err := compare.New()
	if err != nil {
		fmt.Println("error creating secure comparator object")
		return
	}

	err = sc.Append([]byte(os.Args[1]))
	if err != nil {
		fmt.Println("error appending secret to secure comparator")
		return
	}

	buf, err := sc.Begin()
	if err != nil {
		fmt.Println("error creating init message")
		return
	}

	for {
		_, err = conn.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			return
		}

		res, err := sc.Result()
		if err != nil {
			fmt.Println("error geting result from secure comparator")
			return
		}

		if compare.COMPARE_NOT_READY == res {
			buf = make([]byte, 10240)
			readed_bytes, err := conn.Read(buf)
			if err != nil {
				fmt.Println("error reading bytes from socket")
				return
			}
			buffer, err := sc.Proceed(buf[:readed_bytes])
			if nil != err {
				fmt.Println("error unwraping message")
				return
			}
			buf = buffer
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
