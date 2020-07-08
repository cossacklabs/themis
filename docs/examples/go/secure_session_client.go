package main

import (
	"encoding/base64"
	"fmt"
	"net"
	"os"

	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/session"
)

type callbacks struct {
}

func (clb *callbacks) GetPublicKeyForId(ss *session.SecureSession, id []byte) *keys.PublicKey {
	decodedID, err := base64.StdEncoding.DecodeString(string(id[:]))
	if nil != err {
		return nil
	}
	return &keys.PublicKey{Value: decodedID}
}

func (clb *callbacks) StateChanged(ss *session.SecureSession, state int) {

}

func sendWrappedMessage(c net.Conn, ss *session.SecureSession, message string) bool {
	buf, err := ss.Wrap([]byte(message))
	if nil != err {
		fmt.Println("error wrapping message")
		os.Exit(1)
	}
	_, err = c.Write(buf)
	if err != nil {
		fmt.Println("error writing bytes from socket")
		os.Exit(1)
	}
	return true
}

func receiveUnwrappedMessage(c net.Conn, ss *session.SecureSession) string {
	buf := make([]byte, 10240)
	readBytes, err := c.Read(buf)
	if err != nil {
		fmt.Println("error reading bytes from socket")
		os.Exit(1)
	}
	buf, _, err = ss.Unwrap(buf[:readBytes])
	if nil != err {
		fmt.Println("error unwraping message")
		os.Exit(1)
	}
	return string(buf[:])
}

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Println("connection error")
		os.Exit(1)
	}
	clientKeyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		fmt.Println("error generating key pair")
		os.Exit(1)
	}
	ss, err := session.New([]byte(base64.StdEncoding.EncodeToString(clientKeyPair.Public.Value)), clientKeyPair.Private, &callbacks{})
	if err != nil {
		fmt.Println("error creating secure session object")
		os.Exit(1)
	}

	buf, err := ss.ConnectRequest()
	if err != nil {
		fmt.Println("error creating connection request")
		os.Exit(1)
	}

	for {
		_, err = conn.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			os.Exit(1)
		}

		buf = make([]byte, 10240)
		readBytes, err := conn.Read(buf)
		if err != nil {
			fmt.Println("error reading bytes from socket")
			os.Exit(1)
		}
		buffer, sendPeer, err := ss.Unwrap(buf[:readBytes])
		if nil != err {
			fmt.Println("error unwraping message")
			os.Exit(1)
		}
		buf = buffer
		if !sendPeer {
			break
		}
	}

	if sendWrappedMessage(conn, ss, "This is test themis secure session message") {
		mes := receiveUnwrappedMessage(conn, ss)
		if "" != mes {
			fmt.Println("Received:", mes)
			sendWrappedMessage(conn, ss, "finish")
		}
	}
}
