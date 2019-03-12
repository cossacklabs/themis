package main

import (
	"bufio"
	"bytes"
	"encoding/base64"
	"errors"
	"fmt"
	"github.com/cossacklabs/themis/gothemis/keys"
	"github.com/cossacklabs/themis/gothemis/message"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"strings"
)

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
		return nil, errors.New(string(data))
	}
	return data, nil
}

func createSecureMessage(inputBuffer *bufio.Reader) (*message.SecureMessage, string, error) {
	fmt.Println("Type your settings from https://themis.cossacklabs.com/interactive-simulator/setup/")

	fmt.Println("JSON endpoint: ")
	endpoint, err := inputBuffer.ReadString('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read JSON endpoint: %s", err)
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

	fmt.Println("Server public key in base64 format:")
	serverPublic, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		err = fmt.Errorf("Failed to read server public key: %s", err)
		return nil, "", err
	}

	serverPublic = bytes.TrimRight(serverPublic, "\r\n")
	serverPublic, err = base64.StdEncoding.DecodeString(string(serverPublic))
	if err != nil {
		err = fmt.Errorf("Incorrect base64 format for public key: %s", err)
		return nil, "", err
	}

	secureMessage := message.New(
		&keys.PrivateKey{Value: bytes.TrimRight(clientPrivate, "\r\n")},
		&keys.PublicKey{Value: serverPublic})

	return secureMessage, endpoint, nil
}

func runSecureMessage(secureMessage *message.SecureMessage, endpoint string, inputBuffer *bufio.Reader) error {
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

		wrapped, err := secureMessage.Wrap(line)
		if err != nil {
			err = fmt.Errorf("Failed to encrypt message: %s", err)
			return err
		}
		data, err := sendMessage(wrapped, endpoint)
		if err != nil {
			err = fmt.Errorf("Failed to send message: %s", err)
			return err
		}
		unwrapped, err := secureMessage.Unwrap(data)
		if err != nil {
			err = fmt.Errorf("Failed to decrypt message: %s", err)
			return err
		}
		fmt.Println(string(unwrapped))
	}
}

func main() {
	inputBuffer := bufio.NewReader(os.Stdin)

	secureMessage, endpoint, err := createSecureMessage(inputBuffer)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	err = runSecureMessage(secureMessage, endpoint, inputBuffer)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
