package main

import (
	"encoding/base64"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/session"
	"net"
)

type callbacks struct {
}

func (clb *callbacks) GetPublicKeyForId(ss *session.SecureSession, id []byte) *keys.PublicKey {
	decoded_id, err := base64.StdEncoding.DecodeString(string(id[:]))
	if nil != err {
		return nil
	}
	return &keys.PublicKey{decoded_id}
}

func (clb *callbacks) StateChanged(ss *session.SecureSession, state int) {

}

func send_wrapped_message(c net.Conn, ss *session.SecureSession, message string) bool {
	buf, err := ss.Wrap([]byte(message))
	if nil != err {
		fmt.Println("error wrapping message")
		return false
	}
	_, err = c.Write(buf)
	if err != nil {
		fmt.Println("error writing bytes from socket")
		return false
	}
	return true
}

func receive_unwrapped_message(c net.Conn, ss *session.SecureSession) string {
	buf := make([]byte, 10240)
	readed_bytes, err := c.Read(buf)
	if err != nil {
		fmt.Println("error reading bytes from socket")
		return ""
	}
	buf, _, err = ss.Unwrap(buf[:readed_bytes])
	if nil != err {
		fmt.Println("error unwraping message")
		return ""
	}
	return string(buf[:])
}

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Println("connection error")
		return
	}
	client_keypair, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		fmt.Println("error generating key pair")
		return
	}
	ss, err := session.New([]byte(base64.StdEncoding.EncodeToString(client_keypair.Public.Value)), client_keypair.Private, &callbacks{})
	if err != nil {
		fmt.Println("error creating secure session object")
		return
	}

	buf, err := ss.ConnectRequest()
	if err != nil {
		fmt.Println("error creating connection request")
		return
	}

	for {
		_, err = conn.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			return
		}

		buf = make([]byte, 10240)
		readed_bytes, err := conn.Read(buf)
		if err != nil {
			fmt.Println("error reading bytes from socket")
			return
		}
		buffer, send_peer, err := ss.Unwrap(buf[:readed_bytes])
		if nil != err {
			fmt.Println("error unwraping message")
			return
		}
		buf = buffer
		if !send_peer {
			break
		}
	}

	if send_wrapped_message(conn, ss, "This is test themis secure session message") {
		mes := receive_unwrapped_message(conn, ss)
		if "" != mes {
			fmt.Println("Received:", mes)
			send_wrapped_message(conn, ss, "finish")
		}
	}
}
