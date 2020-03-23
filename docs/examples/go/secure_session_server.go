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

func connectionHandler(c net.Conn, serverID string, serverPrivateKey *keys.PrivateKey) {
	ss, err := session.New([]byte(serverID), serverPrivateKey, &callbacks{})
	if err != nil {
		fmt.Println("error creating secure session object")
		os.Exit(1)
	}
	for {
		buf := make([]byte, 10240)
		readBytes, err := c.Read(buf)
		if err != nil {
			fmt.Println("error reading bytes from socket")
			os.Exit(1)
		}
		buf, sendPeer, err := ss.Unwrap(buf[:readBytes])
		if nil != err {
			fmt.Println("error unwraping message")
			os.Exit(1)
		}
		if !sendPeer {
			if "finish" == string(buf[:]) {
				return
			}
			fmt.Println("Receied:", string(buf[:]))
			buf, err = ss.Wrap(buf)
			if nil != err {
				fmt.Println("error unwraping message")
				os.Exit(1)
			}
		}
		_, err = c.Write(buf)
		if err != nil {
			fmt.Println("error writing bytes from socket")
			os.Exit(1)
		}
	}
}

func main() {
	l, err := net.Listen("tcp", ":8080")
	if err != nil {
		fmt.Println("listen error")
		os.Exit(1)
	}
	serverKeyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		fmt.Println("error generating key pair")
		os.Exit(1)
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Println("accepting error")
			os.Exit(1)
		}

		go connectionHandler(conn, base64.StdEncoding.EncodeToString(serverKeyPair.Public.Value), serverKeyPair.Private)
	}
}
