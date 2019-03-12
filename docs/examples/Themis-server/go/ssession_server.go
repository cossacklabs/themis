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

func createSecureSession(inputBuffer *bufio.Reader) (*session.SecureSession, string, error) {
	fmt.Println("Type your settings from https://themis.cossacklabs.com/interactive-simulator/setup/")

	fmt.Println("JSON endpoint: ")
	endpoint, err := inputBuffer.ReadString('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read endpoint URL: %s", err)
		return nil, "", err
	}
	endpoint = strings.TrimRight(endpoint, "\n\r")

	fmt.Println("Your private key in base64 format:")
	clientPrivate, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read user private key: %s", err)
		return nil, "", err
	}
	clientPrivate, err = base64.StdEncoding.DecodeString(string(clientPrivate))
	if err != nil {
		err = fmt.Errorf("Incorrect base64 format for private key: %s", err)
		return nil, "", err
	}

	fmt.Println("User ID:")
	clientID, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read user ID: %s", err)
		return nil, "", err
	}

	fmt.Println("Server ID:")
	serverID, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read server ID: %s", err)
		return nil, "", err
	}

	fmt.Println("Server public key in base64 format:")
	serverPublic, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read server public key: %s", err)
		return nil, "", err
	}
	serverPublic, err = base64.StdEncoding.DecodeString(string(serverPublic))
	if err != nil {
		err = fmt.Errorf("Incorrect base64 format for public key: %s", err)
		return nil, "", err
	}

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
		err = fmt.Errorf("Cannot create Secure Session: %s", err)
		return nil, "", err
	}

	return clientSession, endpoint, nil
}

func runSecureSession(clientSession *session.SecureSession, endpoint string, inputBuffer *bufio.Reader) error {
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
				err = fmt.Errorf("Failed to send message: %s", err)
				return err
			}
			ch <- data
		case <-quitChannel:
			isEstablished = true
		}
	}
	fmt.Println("Session established")

	for {
		fmt.Println("Print message to send (or \"quit\" to stop):")
		line, _, err := inputBuffer.ReadLine()
		if err != nil {
			err = fmt.Errorf("Failed to read message: %s", err)
			return err
		}
		if bytes.Equal(line, []byte("quit")) {
			return nil
		}
		wrapped, err := clientSession.Wrap(line)
		if err != nil {
			err = fmt.Errorf("Failed to read message: %s", err)
			return err
		}
		data, err := sendMessage(wrapped, endpoint)
		if err != nil {
			err = fmt.Errorf("Failed to send message: %s", err)
			return err
		}
		unwrapped, _, err := clientSession.Unwrap(data)
		if err != nil {
			err = fmt.Errorf("Failed to decrypt message: %s", err)
			return err
		}
		fmt.Println(string(unwrapped))
	}
}

func main() {
	inputBuffer := bufio.NewReader(os.Stdin)

	clientSession, endpoint, err := createSecureSession(inputBuffer)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	err = runSecureSession(clientSession, endpoint, inputBuffer)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
