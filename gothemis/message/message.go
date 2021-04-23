package message

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/themis_error.h>
#include <themis/secure_message.h>

enum {
	SecureMessageEncrypt,
	SecureMessageDecrypt,
	SecureMessageSign,
	SecureMessageVerify,
};

static bool get_message_size(const void *priv, size_t priv_len, const void *public, size_t pub_len, const void *message, size_t message_len, int mode, size_t *out_len)
{
	themis_status_t res = THEMIS_NOT_SUPPORTED;

	switch (mode) {
	case SecureMessageEncrypt:
		res = themis_secure_message_encrypt(priv, priv_len, public, pub_len, message, message_len, NULL, out_len);
		break;
	case SecureMessageDecrypt:
		res = themis_secure_message_decrypt(priv, priv_len, public, pub_len, message, message_len, NULL, out_len);
		break;
	case SecureMessageSign:
		res = themis_secure_message_sign(priv, priv_len, message, message_len, NULL, out_len);
		break;
	case SecureMessageVerify:
		res = themis_secure_message_verify(public, pub_len, message, message_len, NULL, out_len);
		break;
	}

	return THEMIS_BUFFER_TOO_SMALL == res;
}

static bool process(const void *priv, size_t priv_len, const void *public, size_t pub_len, const void *message, size_t message_len, int mode, void *out, size_t *out_len)
{
	themis_status_t res = THEMIS_NOT_SUPPORTED;

	switch (mode) {
	case SecureMessageEncrypt:
		res = themis_secure_message_encrypt(priv, priv_len, public, pub_len, message, message_len, out, out_len);
		break;
	case SecureMessageDecrypt:
		res = themis_secure_message_decrypt(priv, priv_len, public, pub_len, message, message_len, out, out_len);
		break;
	case SecureMessageSign:
		res = themis_secure_message_sign(priv, priv_len, message, message_len, out, out_len);
		break;
	case SecureMessageVerify:
		res = themis_secure_message_verify(public, pub_len, message, message_len, out, out_len);
		break;
	}

	return THEMIS_SUCCESS == res;
}

*/
import "C"
import (
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
	"github.com/cossacklabs/themis/gothemis/keys"
)

const (
	secureMessageEncrypt = iota
	secureMessageDecrypt
	secureMessageSign
	secureMessageVerify
)

// Errors returned by Secure Message.
var (
	ErrEncryptMessage    = errors.New("failed to encrypt message")
	ErrDecryptMessage    = errors.New("failed to decrypt message")
	ErrSignMessage       = errors.New("failed to sign message")
	ErrVerifyMessage     = errors.New("failed to verify message")
	ErrProcessMessage    = errors.New("failed to process message")
	ErrGetOutputSize     = errors.New("failed to get output size")
	ErrMissingMessage    = errors.NewWithCode(errors.InvalidParameter, "empty message for Secure Cell")
	ErrMissingPublicKey  = errors.NewWithCode(errors.InvalidParameter, "empty peer public key for Secure Message")
	ErrMissingPrivateKey = errors.NewWithCode(errors.InvalidParameter, "empty private key for Secure Message")
	ErrOutOfMemory       = errors.NewWithCode(errors.NoMemory, "Secure Message cannot allocate enough memory")
	// Deprecated: Since 0.14. Use ErrOutOfMemory instead.
	ErrOverflow          = ErrOutOfMemory
)

// SecureMessage provides a sequence-independent, stateless, contextless messaging system.
type SecureMessage struct {
	private    *keys.PrivateKey
	peerPublic *keys.PublicKey
}

// New makes a new Secure Message context.
// You need to specify both keys for encrypt-decrypt mode.
// Private key is required for signing messages. Public key is required for verifying messages
func New(private *keys.PrivateKey, peerPublic *keys.PublicKey) *SecureMessage {
	return &SecureMessage{private, peerPublic}
}

// C returns sizes as size_t but Go expresses buffer lengths as int.
// Make sure that all sizes are representable in Go and there is no overflows.
func sizeOverflow(n C.size_t) bool {
	const maxInt = int(^uint(0) >> 1)
	return n > C.size_t(maxInt)
}

func messageProcess(private *keys.PrivateKey, peerPublic *keys.PublicKey, message []byte, mode int) ([]byte, error) {
	if nil == message || 0 == len(message) {
		return nil, ErrMissingMessage
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

	var outputLength C.size_t
	if !bool(C.get_message_size(priv,
		privLen,
		pub,
		pubLen,
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		C.int(mode),
		&outputLength)) {
		return nil, ErrGetOutputSize
	}
	if sizeOverflow(outputLength) {
		return nil, ErrOutOfMemory
	}

	output := make([]byte, int(outputLength), int(outputLength))
	if !bool(C.process(priv,
		privLen,
		pub,
		pubLen,
		unsafe.Pointer(&message[0]),
		C.size_t(len(message)),
		C.int(mode),
		unsafe.Pointer(&output[0]),
		&outputLength)) {
		switch mode {
		case secureMessageEncrypt:
			return nil, ErrEncryptMessage
		case secureMessageDecrypt:
			return nil, ErrDecryptMessage
		case secureMessageSign:
			return nil, ErrSignMessage
		case secureMessageVerify:
			return nil, ErrVerifyMessage
		default:
			return nil, ErrProcessMessage
		}
	}

	return output[:outputLength], nil
}

// Wrap encrypts the provided message.
func (sm *SecureMessage) Wrap(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, ErrMissingPrivateKey
	}

	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, ErrMissingPublicKey
	}
	return messageProcess(sm.private, sm.peerPublic, message, secureMessageEncrypt)
}

// Unwrap decrypts the encrypted message.
func (sm *SecureMessage) Unwrap(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, ErrMissingPrivateKey
	}

	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, ErrMissingPublicKey
	}
	return messageProcess(sm.private, sm.peerPublic, message, secureMessageDecrypt)
}

// Sign signs the provided message and returns it signed.
func (sm *SecureMessage) Sign(message []byte) ([]byte, error) {
	if nil == sm.private || 0 == len(sm.private.Value) {
		return nil, ErrMissingPrivateKey
	}

	return messageProcess(sm.private, nil, message, secureMessageSign)
}

// Verify checks the signature on the message and returns the original message.
func (sm *SecureMessage) Verify(message []byte) ([]byte, error) {
	if nil == sm.peerPublic || 0 == len(sm.peerPublic.Value) {
		return nil, ErrMissingPublicKey
	}

	return messageProcess(nil, sm.peerPublic, message, secureMessageVerify)
}
