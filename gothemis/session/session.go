package session

/*
#cgo LDFLAGS: -lthemis -lsoter
#include "session.h"
extern const int GOTHEMIS_INVALID_PARAMETER;
extern const int GOTHEMIS_BUFFER_TOO_SMALL;
extern const int GOTHEMIS_SUCCESS;
extern const int GOTHEMIS_SSESSION_GET_PUB_FOR_ID_ERROR;
extern const int GOTHEMIS_SSESSION_SEND_OUTPUT_TO_PEER;
*/
import "C"
import (
	"reflect"
	"runtime"
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
	"github.com/cossacklabs/themis/gothemis/keys"
)

// Secure Session states.
const (
	StateIdle = iota
	StateNegotiating
	StateEstablished
)

// Secure Session states.
//
// Deprecated: Since 0.11. Use "session.State..." constants instead.
const (
	STATE_IDLE        = StateIdle
	STATE_NEGOTIATING = StateNegotiating
	STATE_ESTABLISHED = StateEstablished
)

// Errors returned by Secure Session.
var (
	ErrCreateSession             = errors.New("failed to create secure session object")
	ErrDestroySession            = errors.New("failed to destroy secure session object")
	ErrMessageSize               = errors.New("failed to get message size")
	ErrMessageData               = errors.New("failed to process message data")
	ErrNoPublicKey               = errors.NewCallbackError("failed to get public key (get_public_key_by_id callback error)")
	ErrBadRemoteIDLength         = errors.NewCallbackError("incorrect remote id length (0)")
	ErrGetRemoteID               = errors.NewCallbackError("failed to get session remote id")
	ErrMissingClientID           = errors.NewWithCode(errors.InvalidParameter, "empty client ID for Secure Session")
	ErrMissingPrivateKey         = errors.NewWithCode(errors.InvalidParameter, "empty client private key for Secure Session")
	ErrMissingMessage            = errors.NewWithCode(errors.InvalidParameter, "empty message for Secure Session")
	ErrOutOfMemory               = errors.NewWithCode(errors.NoMemory, "Secure Session cannot allocate enough memory")
	// Deprecated: Since 0.14. Use ErrOutOfMemory instead.
	ErrOverflow                  = ErrOutOfMemory
)

// SessionCallbacks implements a delegate for SecureSession.
type SessionCallbacks interface {
	GetPublicKeyForId(ss *SecureSession, id []byte) *keys.PublicKey
	StateChanged(ss *SecureSession, state int)
}

// SecureSession is a lightweight mechanism for securing any kind of network communication
// (both private and public networks, including the Internet). It is protocol-agnostic and
// operates on the 5th layer of the network OSI model (the session layer).
type SecureSession struct {
	ctx   *C.struct_session_with_callbacks_type
	clb   SessionCallbacks
	state int
}

func finalize(ss *SecureSession) {
	ss.Close()
}

// C returns sizes as size_t but Go expresses buffer lengths as int.
// Make sure that all sizes are representable in Go and there is no overflows.
func sizeOverflow(n C.size_t) bool {
	const maxInt = int(^uint(0) >> 1)
	return n > C.size_t(maxInt)
}

// New makes a new Secure Session with provided peer ID, private key, and callbacks.
func New(id []byte, signKey *keys.PrivateKey, callbacks SessionCallbacks) (*SecureSession, error) {
	ss := &SecureSession{clb: callbacks}

	if nil == id || 0 == len(id) {
		return nil, ErrMissingClientID
	}

	if nil == signKey || 0 == len(signKey.Value) {
		return nil, ErrMissingPrivateKey
	}

	ss.ctx = C.session_init(unsafe.Pointer(&id[0]),
		C.size_t(len(id)),
		unsafe.Pointer(&signKey.Value[0]),
		C.size_t(len(signKey.Value)))

	if ss.ctx == nil {
		return nil, ErrCreateSession
	}

	runtime.SetFinalizer(ss, finalize)

	return ss, nil
}

// Close destroys a Secure Session.
func (ss *SecureSession) Close() error {
	if nil != ss.ctx {
		if bool(C.session_destroy(ss.ctx)) {
			ss.ctx = nil
		} else {
			return ErrDestroySession
		}
	}

	return nil
}

//export onPublicKeyForId
func onPublicKeyForId(ssCtx unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, keyPtr unsafe.Pointer, keyLen C.size_t) int {
	var ss *SecureSession
	if sizeOverflow(idLen) {
		return int(C.THEMIS_NO_MEMORY)
	}
	id := C.GoBytes(idPtr, C.int(idLen))
	ss = (*SecureSession)(unsafe.Pointer(uintptr(ssCtx) - unsafe.Offsetof(ss.ctx)))

	pub := ss.clb.GetPublicKeyForId(ss, id)
	if nil == pub {
		return int(C.GOTHEMIS_INVALID_PARAMETER)
	}

	if len(pub.Value) > int(keyLen) {
		return int(C.GOTHEMIS_BUFFER_TOO_SMALL)
	}
	sliceHeader := reflect.SliceHeader{Data: uintptr(keyPtr), Len: int(keyLen), Cap: int(keyLen)}
	key := *(*[]byte)(unsafe.Pointer(&sliceHeader))
	copy(key, pub.Value)
	return int(C.GOTHEMIS_SUCCESS)
}

//export onStateChanged
func onStateChanged(ssCtx unsafe.Pointer, event int) {
	var ss *SecureSession
	ss = (*SecureSession)(unsafe.Pointer(uintptr(ssCtx) - unsafe.Offsetof(ss.ctx)))
	ss.state = event
	ss.clb.StateChanged(ss, event)
}

// GetState returns current state of Secure Session.
func (ss *SecureSession) GetState() int {
	return ss.state
}

// ConnectRequest generates a connection request that must be sent to the remote peer.
func (ss *SecureSession) ConnectRequest() ([]byte, error) {
	var reqLen C.size_t

	if !bool(C.session_connect_size(ss.ctx,
		&reqLen)) {
		return nil, ErrMessageSize
	}
	if sizeOverflow(reqLen) {
		return nil, ErrOutOfMemory
	}

	req := make([]byte, reqLen)
	if !bool(C.session_connect(&ss.ctx,
		unsafe.Pointer(&req[0]),
		reqLen)) {
		return nil, ErrMessageData
	}

	return req, nil
}

// Wrap encrypts the provided data for the peer.
func (ss *SecureSession) Wrap(data []byte) ([]byte, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, ErrMissingMessage
	}

	if !bool(C.session_wrap_size(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		&outLen)) {
		return nil, ErrMessageSize
	}
	if sizeOverflow(outLen) {
		return nil, ErrOutOfMemory
	}

	out := make([]byte, outLen)
	if !bool(C.session_wrap(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		unsafe.Pointer(&out[0]),
		outLen)) {
		return nil, ErrMessageData
	}

	return out, nil
}

// Unwrap decrypts the encrypted data from the peer.
// It is also used for connection negotiation.
func (ss *SecureSession) Unwrap(data []byte) ([]byte, bool, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, false, ErrMissingMessage
	}

	res := C.session_unwrap_size(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		&outLen)
	switch {
	case (C.GOTHEMIS_SUCCESS == res) && (0 == outLen):
		return nil, false, nil
	case (C.GOTHEMIS_SSESSION_GET_PUB_FOR_ID_ERROR == res):
		return nil, false, ErrNoPublicKey
	case (C.GOTHEMIS_BUFFER_TOO_SMALL != res):
		return nil, false, ErrMessageSize
	}
	if sizeOverflow(outLen) {
		return nil, false, ErrOutOfMemory
	}

	out := make([]byte, outLen)

	res = C.session_unwrap(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		unsafe.Pointer(&out[0]),
		outLen)

	switch {
	case (C.GOTHEMIS_SUCCESS == res) && (0 == outLen):
		return nil, false, nil
	case (C.GOTHEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (0 < outLen):
		return out, true, nil
	case (C.GOTHEMIS_SUCCESS == res) && (0 < outLen):
		return out, false, nil
	case (C.GOTHEMIS_SSESSION_GET_PUB_FOR_ID_ERROR == res):
		return nil, false, ErrNoPublicKey
	}

	return nil, false, ErrMessageData
}

// GetRemoteID returns ID of the remote peer.
func (ss *SecureSession) GetRemoteID() ([]byte, error) {
	// secure_session_get_remote_id
	var outLength C.size_t
	if C.secure_session_get_remote_id(ss.ctx.session, nil, &outLength) != C.THEMIS_BUFFER_TOO_SMALL {
		return nil, ErrGetRemoteID
	}
	if outLength == 0 {
		return nil, ErrBadRemoteIDLength
	}
	if sizeOverflow(outLength) {
		return nil, ErrOutOfMemory
	}
	out := make([]byte, int(outLength))
	if C.secure_session_get_remote_id(ss.ctx.session, (*C.uint8_t)(&out[0]), &outLength) != C.THEMIS_SUCCESS {
		return nil, ErrGetRemoteID
	}
	return out, nil
}

// GetRemoteId returns ID of the remote peer.
//
// Deprecated: Since 0.11. Use GetRemoteID() instead.
func (ss *SecureSession) GetRemoteId() ([]byte, error) {
	return ss.GetRemoteID()
}
