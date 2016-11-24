package message

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/error.h>
#include <themis/secure_message.h>

static bool get_message_size(const void *priv, size_t priv_len, const void *public, size_t pub_len, const void *message, size_t message_len, bool is_wrap, size_t *out_len)
{
	themis_status_t res;

	if (is_wrap)
	{
		res = themis_secure_message_wrap(priv, priv_len, public, pub_len, message, message_len, NULL, out_len);
	}
	else
	{
		res = themis_secure_message_unwrap(priv, priv_len, public, pub_len, message, message_len, NULL, out_len);
	}

	return THEMIS_BUFFER_TOO_SMALL == res;
}

static bool process(const void *priv, size_t priv_len, const void *public, size_t pub_len, const void *message, size_t message_len, bool is_wrap, void *out, size_t out_len)
{
	themis_status_t res;

	if (is_wrap)
	{
		res = themis_secure_message_wrap(priv, priv_len, public, pub_len, message, message_len, out, &out_len);
	}
	else
	{
		res = themis_secure_message_unwrap(priv, priv_len, public, pub_len, message, message_len, out, &out_len);
	}

	return THEMIS_SUCCESS == res;
}

*/
import "C"
import (
	"github.com/cossacklabs/themis/gothemis/errors"
	"github.com/cossacklabs/themis/gothemis/keys"
	"unsafe"
)

type SecureMessage struct {
	private    *keys.PrivateKey
	peerPublic *keys.PublicKey
}

func New(private *keys.PrivateKey, peerPublic *keys.PublicKey) *SecureMessage {
	return &SecureMessage{private, peerPublic}
}

func messageProcess(private *keys.PrivateKey, peerPublic *keys.PublicKey, message []byte, is_wrap bool) ([]byte, error) {
	if nil == message || 0 == len(message) {
		return nil, errors.New("No message was provided")
	}

	var priv, pub unsafe.Pointer
	var privLen, pubLen C.size_t
	priv = nil
	pub = nil
	privLen = 0
	pubLen = 0

	if nil != private && 0 < len(private.Value) {
		priv = unsafe.Pointer(&private.Value[0])
		privLen = C.size_t(len(private.Value))
	}

	if nil != peerPublic && 0 < len(peerPublic.Value) {
		pub = unsafe.Pointer(&peerPublic.Value[0])
		pubLen = C.size_t(len(peerPublic.Value))
	}

	var output_length C.size_t
	if !bool(C.get_message_size(priv,
		privLen,
		pub,
		pubLen,
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		C.bool(is_wrap),
		&output_length)) {
		return nil, errors.New("Failed to get output size")
	}

	output := make([]byte, int(output_length), int(output_length))
	if !bool(C.process(priv,
		privLen,
		pub,
		pubLen,
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		C.bool(is_wrap),
		unsafe.Pointer(&output[0]),
		output_length)) {
		if is_wrap {
			return nil, errors.New("Failed to wrap message")
		} else {
			return nil, errors.New("Failed to unwrap message")
		}

	}

	return output, nil
}

func (sm *SecureMessage) Wrap(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, errors.New("Private key was not provided")
	}

	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, errors.New("Peer public key was not provided")
	}
	return messageProcess(sm.private, sm.peerPublic, message, true)
}

func (sm *SecureMessage) Unwrap(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, errors.New("Private key was not provided")
	}

	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, errors.New("Peer public key was not provided")
	}
	return messageProcess(sm.private, sm.peerPublic, message, false)
}

func (sm *SecureMessage) Sign(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, errors.New("Private key was not provided")
	}

	return messageProcess(sm.private, nil, message, true)
}

func (sm *SecureMessage) Verify(message []byte) ([]byte, error) {
	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, errors.New("Peer public key was not provided")
	}

	return messageProcess(nil, sm.peerPublic, message, false)
}
