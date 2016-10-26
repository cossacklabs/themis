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

func connectionHandler(c net.Conn, server_id string, server_private_key *keys.PrivateKey) {
	ss, err := session.New([]byte(server_id), server_private_key, &callbacks{})
	if err != nil {
		fmt.Println("error creating secure session object")
		return
	}
	for {
		buf := make([]byte, 10240)
		readed_bytes, err := c.Read(buf)
		if err != nil {
			fmt.Println("error reading bytes from socket")
			return
		}
		buf, send_peer, err := ss.Unwrap(buf[:readed_bytes])
		if nil != err {
			fmt.Println("error unwraping message")
			return
		}
		if !send_peer {
			if "finish" == string(buf[:]) {
				return
			}
			fmt.Println("Receied:", string(buf[:]))
			buf, err = ss.Wrap(buf)
			if nil != err {
				fmt.Println("error unwraping message")
				return
			}
		}
		_, err = c.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			return
		}
	}
}

func main() {
	l, err := net.Listen("tcp", ":8080")
	if err != nil {
		fmt.Println("listen error")
		return
	}
	server_keypair, err := keys.New(keys.KEYTYPE_EC)
	if err != nil {
		fmt.Println("error generating key pair")
		return
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("accepting error")
			return
		}

		go connectionHandler(conn, base64.StdEncoding.EncodeToString(server_keypair.Public.Value), server_keypair.Private)
	}
}
