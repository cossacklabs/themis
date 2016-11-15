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

func send_message(message []byte, endpoint string) ([]byte, error) {
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
	input_buffer := bufio.NewReader(os.Stdin)
	fmt.Println("Type your settings from https://themis.cossacklabs.com/interactive-simulator/setup/")

	fmt.Println("JSON endpoint: ")
	endpoint, err := input_buffer.ReadString('\n')
	endpoint = strings.TrimRight(endpoint, "\n\r")

	fmt.Println("Your private key in base64 format:")
	client_private, err := input_buffer.ReadBytes('\n')
	client_private, err = base64.StdEncoding.DecodeString(string(client_private))
	if err != nil {
		fmt.Println("Incorrect base64 format for private key")
		return
	}

	fmt.Println("Server public key in base64 format:")
	server_public, err := input_buffer.ReadBytes('\n')
	server_public = bytes.TrimRight(server_public, "\r\n")
	server_public, err = base64.StdEncoding.DecodeString(string(server_public))
	secure_message := message.New(
		&keys.PrivateKey{bytes.TrimRight(client_private, "\r\n")},
		&keys.PublicKey{server_public})

	for {
		fmt.Println("Print message to send (or quit to stop):")
		line, _, err := input_buffer.ReadLine()
		if err != nil {
			fmt.Println(err)
			return
		}
		if bytes.Equal(line, []byte("quit")) {
			return
		}

		wrapped, err := secure_message.Wrap(line)
		if err != nil {
			fmt.Println("Error in wraping", err)
			return
		}
		data, err := send_message(wrapped, endpoint)
		if err != nil {
			fmt.Println("Error occured:", err)
			return
		}
		unwrapped, err := secure_message.Unwrap(data)
		fmt.Println(string(unwrapped))
	}
}
