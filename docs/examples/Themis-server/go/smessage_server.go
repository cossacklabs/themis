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

func main() {
	inputBuffer := bufio.NewReader(os.Stdin)
	fmt.Println("Type your settings from https://themis.cossacklabs.com/interactive-simulator/setup/")

	fmt.Println("JSON endpoint: ")
	endpoint, err := inputBuffer.ReadString('\n')
	if err != nil {
		fmt.Println("Failed to read JSON endpoint:", err)
		return
	}
	endpoint = strings.TrimRight(endpoint, "\n\r")

	fmt.Println("Your private key in base64 format:")
	clientPrivate, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		fmt.Println("Failed to read user private key:", err)
		return
	}
	clientPrivate, err = base64.StdEncoding.DecodeString(string(clientPrivate))
	if err != nil {
		fmt.Println("Incorrect base64 format for private key:", err)
		return
	}

	fmt.Println("Server public key in base64 format:")
	serverPublic, err := inputBuffer.ReadBytes('\n')
	if err != nil {
		fmt.Println("Failed to read server public key:", err)
		return
	}

	serverPublic = bytes.TrimRight(serverPublic, "\r\n")
	serverPublic, err = base64.StdEncoding.DecodeString(string(serverPublic))
	if err != nil {
		fmt.Println("Incorrect base64 format for public key:", err)
		return
	}

	secureMessage := message.New(
		&keys.PrivateKey{Value: bytes.TrimRight(clientPrivate, "\r\n")},
		&keys.PublicKey{Value: serverPublic})

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

		wrapped, err := secureMessage.Wrap(line)
		if err != nil {
			fmt.Println("Error in wraping", err)
			return
		}
		data, err := sendMessage(wrapped, endpoint)
		if err != nil {
			fmt.Println("Error occurred:", err)
			return
		}
		unwrapped, err := secureMessage.Unwrap(data)
		if err != nil {
			fmt.Println("Error unwrapping:", err)
			return
		}
		fmt.Println(string(unwrapped))
	}
}
