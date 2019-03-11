package main

import (
	"bufio"
	"bytes"
	"encoding/base64"
	"errors"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/session"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"strings"
)

type clientTransportCallback struct {
	serverPublic []byte
	serverID     []byte
}

func (clb *clientTransportCallback) GetPublicKeyForId(ss *session.SecureSession, id []byte) *keys.PublicKey {
	if bytes.Equal(id, clb.serverID) {
		return &keys.PublicKey{Value: clb.serverPublic}
	}
	return nil

}

func (clb *clientTransportCallback) StateChanged(ss *session.SecureSession, state int) {

}

func sendMessage(message []byte, endpoint string) ([]byte, error) {
	values := url.Values{}
	values.Add("message", base64.StdEncoding.EncodeToString(message))
	request, err := http.NewRequest("POST", endpoint, bytes.NewBufferString(values.Encode()))
	if err != nil {
		return nil, err
	}
	request.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	response, err := http.DefaultClient.Do(request)
	if err != nil {
		return nil, err
	}
	data, err := ioutil.ReadAll(response.Body)
	if err != nil {
		return nil, err
	}
	response.Body.Close()
	if response.StatusCode > 299 {
		fmt.Println(string(data))
		return nil, errors.New("Response error")
	}
	return data, nil
}

func clientService(client *session.SecureSession, ch chan []byte, finCh chan int) {
	connectionRequest, err := client.ConnectRequest()
	if nil != err {
		return
	}

	ch <- connectionRequest
	for {
		buf := <-ch

		buf, isNegotiation, err := client.Unwrap(buf)
		if nil != err {
			return
		}

		if isNegotiation {
			ch <- buf
			continue
		}

		break
	}
	finCh <- 1
}

func main() {
	inputBuffer := bufio.NewReader(os.Stdin)
	fmt.Println("Type your settings from https://themis.cossacklabs.com/interactive-simulator/setup/")

	fmt.Println("JSON endpoint: ")
	endpoint, err := inputBuffer.ReadString('\n')
	endpoint = strings.TrimRight(endpoint, "\n\r")

	fmt.Println("Your private key in base64 format:")
	clientPrivate, err := inputBuffer.ReadBytes('\n')
	clientPrivate, err = base64.StdEncoding.DecodeString(string(clientPrivate))
	if err != nil {
		fmt.Println("Incorrect base64 format for private key")
		return
	}

	fmt.Println("User_id:")
	clientID, err := inputBuffer.ReadBytes('\n')

	fmt.Println("Server_id:")
	serverID, err := inputBuffer.ReadBytes('\n')

	fmt.Println("Server public key in base64 format:")
	serverPublic, err := inputBuffer.ReadBytes('\n')
	serverPublic, err = base64.StdEncoding.DecodeString(string(serverPublic))
	// init callback structure
	cb := clientTransportCallback{
		serverPublic,
		bytes.TrimRight(serverID, "\r\n")}

	// create session object
	clientSession, err := session.New(
		bytes.TrimRight(clientID, "\r\n"),
		&keys.PrivateKey{Value: bytes.TrimRight(clientPrivate, "\r\n")},
		&cb)
	if err != nil {
		fmt.Println("Session creation error")
		return
	}

	ch := make(chan []byte)
	quitChannel := make(chan int)
	go clientService(clientSession, ch, quitChannel)
	isEstablished := false
	fmt.Println("Initialize session")
	for !isEstablished {
		select {
		case data := <-ch:
			data, err := sendMessage(data, endpoint)
			if err != nil {
				fmt.Println("Error -", err)
				return
			}
			ch <- data
		case <-quitChannel:
			isEstablished = true
		}
	}
	fmt.Println("Session established")
	for {
		fmt.Println("Print message to send (or quit to stop):")
		line, _, err := inputBuffer.ReadLine()
		if err != nil {
			fmt.Println(err)
			return
		}
		if bytes.Equal(line, []byte("quit")) {
			return
		}
		wrapped, err := clientSession.Wrap(line)
		data, err := sendMessage(wrapped, endpoint)
		if err != nil {
			fmt.Println("Error occurred:", err)
			return
		}
		unwrapped, _, err := clientSession.Unwrap(data)
		fmt.Println(string(unwrapped))
	}
}
