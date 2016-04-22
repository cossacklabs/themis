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

type client_transport_callback struct {
	server_public []byte
	server_id     []byte
}

func (clb *client_transport_callback) GetPublicKeyForId(ss *session.SecureSession, id []byte) *keys.PublicKey {
	if bytes.Equal(id, clb.server_id) {
		return &keys.PublicKey{clb.server_public}
	}
	return nil

}

func (clb *client_transport_callback) StateChanged(ss *session.SecureSession, state int) {

}

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
		fmt.Println(string(data))
		return nil, errors.New("Response error")
	}
	return data, nil
}

func clientService(client *session.SecureSession, ch chan []byte, finCh chan int) {
	connection_request, err := client.ConnectRequest()
	if nil != err {
		return
	}

	ch <- connection_request
	for {
		buf := <-ch

		buf, is_negotiation, err := client.Unwrap(buf)
		if nil != err {
			return
		}

		if is_negotiation {
			ch <- buf
			continue
		}

		break
	}
	finCh <- 1
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

	fmt.Println("User_id:")
	client_id, err := input_buffer.ReadBytes('\n')

	fmt.Println("Server_id:")
	server_id, err := input_buffer.ReadBytes('\n')

	fmt.Println("Server public key in base64 format:")
	server_public, err := input_buffer.ReadBytes('\n')
	server_public, err = base64.StdEncoding.DecodeString(string(server_public))
	// init callback structure
	cb := client_transport_callback{
		server_public,
		bytes.TrimRight(server_id, "\r\n")}

	// create session object
	client_session, err := session.New(
		bytes.TrimRight(client_id, "\r\n"),
		&keys.PrivateKey{bytes.TrimRight(client_private, "\r\n")},
		&cb)
	if err != nil {
		fmt.Println("Session creation error")
		return
	}

	ch := make(chan []byte)
	quit_channel := make(chan int)
	go clientService(client_session, ch, quit_channel)
	is_established := false
	fmt.Println("Initialize session")
	for !is_established {
		select {
		case data := <-ch:
			data, err := send_message(data, endpoint)
			if err != nil {
				fmt.Println("Error -", err)
				return
			}
			ch <- data
		case <-quit_channel:
			is_established = true
		}
	}
	fmt.Println("Session established")
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
		wrapped, err := client_session.Wrap(line)
		data, err := send_message(wrapped, endpoint)
		if err != nil {
			fmt.Println("Error occured:", err)
			return
		}
		unwrapped, _, err := client_session.Unwrap(data)
		fmt.Println(string(unwrapped))
	}
}
