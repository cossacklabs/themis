package session

/*
#cgo LDFLAGS: -lthemis -lsoter
#include "session.h"
*/
import "C"
import (
	"github.com/cossacklabs/themis/gothemis/errors"
	"github.com/cossacklabs/themis/gothemis/keys"
	"runtime"
	"unsafe"
)

const (
	STATE_IDLE        = 0
	STATE_NEGOTIATING = 1
	STATE_ESTABLISHED = 2
)

type SessionCallbacks interface {
	GetPublicKeyForId(ss *SecureSession, id []byte) *keys.PublicKey
	StateChanged(ss *SecureSession, state int)
}

type SecureSession struct {
	ctx   *C.struct_session_with_callbacks_type
	clb   SessionCallbacks
	state int
}

func finalize(ss *SecureSession) {
	ss.Close()
}

func New(id []byte, signKey *keys.PrivateKey, callbacks SessionCallbacks) (*SecureSession, error) {
	ss := &SecureSession{clb: callbacks}

	if nil == id || 0 == len(id) {
		return nil, errors.New("Failed to creating secure session object with empty id")
	}

	if nil == signKey || 0 == len(signKey.Value) {
		return nil, errors.New("Failed to creating secure session object with empty sign key")
	}

	ss.ctx = C.session_init(unsafe.Pointer(&id[0]),
		C.size_t(len(id)),
		unsafe.Pointer(&signKey.Value[0]),
		C.size_t(len(signKey.Value)))

	if ss.ctx == nil {
		return nil, errors.New("Failed to create secure session object")
	}

	runtime.SetFinalizer(ss, finalize)

	return ss, nil
}

func (ss *SecureSession) Close() error {
	if nil != ss.ctx {
		if bool(C.session_destroy(ss.ctx)) {
			ss.ctx = nil
		} else {
			return errors.New("Failed to destroy secure session object")
		}
	}

	return nil
}

//export onPublicKeyForId
func onPublicKeyForId(ssCtx unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, keyPtr unsafe.Pointer, keyLen C.size_t) int {
	var ss *SecureSession
	id := C.GoBytes(idPtr, C.int(idLen))
	ss = (*SecureSession)(unsafe.Pointer(uintptr(ssCtx) - unsafe.Offsetof(ss.ctx)))

	pub := ss.clb.GetPublicKeyForId(ss, id)
	if nil == pub {
		return -2 // THEMIS_INVALID_PARAMETER
	}

	if len(pub.Value) > int(keyLen) {
		return -4 // THEMIS_BUFFER_TOO_SMALL
	}

	key := (*[1 << 30]byte)(keyPtr)[:keyLen:keyLen]
	copy(key, pub.Value)
	return 0
}

//export onStateChanged
func onStateChanged(ssCtx unsafe.Pointer, event int) {
	var ss *SecureSession
	ss = (*SecureSession)(unsafe.Pointer(uintptr(ssCtx) - unsafe.Offsetof(ss.ctx)))
	ss.state = event
	ss.clb.StateChanged(ss, event)
}

func (ss *SecureSession) GetState() int {
	return ss.state
}

func (ss *SecureSession) ConnectRequest() ([]byte, error) {
	var reqLen C.size_t

	if !bool(C.session_connect_size(ss.ctx,
		&reqLen)) {
		return nil, errors.New("Failed to get request size")
	}

	req := make([]byte, reqLen)
	if !bool(C.session_connect(&ss.ctx,
		unsafe.Pointer(&req[0]),
		reqLen)) {
		return nil, errors.New("Failed to generate request")
	}

	return req, nil
}

func (ss *SecureSession) Wrap(data []byte) ([]byte, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, errors.New("Data was not provided")
	}

	if !bool(C.session_wrap_size(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		&outLen)) {
		return nil, errors.New("Failed to get wrapped size")
	}

	out := make([]byte, outLen)
	if !bool(C.session_wrap(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		unsafe.Pointer(&out[0]),
		outLen)) {
		return nil, errors.New("Failed to wrap data")
	}

	return out, nil
}

func (ss *SecureSession) Unwrap(data []byte) ([]byte, bool, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, false, errors.New("Data was not provided")
	}

	res := C.session_unwrap_size(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		&outLen)
	switch {
	case (0 == res) && (0 == outLen):
		return nil, false, nil
	case (-10 == res): // THEMIS_SSESSION_GET_PUB_FOR_ID_ERROR
		return nil, false, errors.NewCallbackError("Failed to get unwraped size (get_public_key_by_id callback error)")
	case (-4 != res): // THEMIS_BUFFER_TOO_SMALL
		return nil, false, errors.New("Failed to get unwrapped size")
	}

	out := make([]byte, outLen)

	res = C.session_unwrap(&ss.ctx,
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		unsafe.Pointer(&out[0]),
		outLen)

	switch {
	case (0 == res) && (0 == outLen):
		return nil, false, nil
	case (1 == res) && (0 < outLen): // THEMIS_SSESSION_SEND_OUTPUT_TO_PEER
		return out, true, nil
	case (0 == res) && (0 < outLen):
		return out, false, nil
	case (-10 == res): // THEMIS_SSESSION_GET_PUB_FOR_ID_ERROR
		return nil, false, errors.NewCallbackError("Failed to unwrap data (get_public_key_by_id callback error)")
	}

	return nil, false, errors.New("Failed to unwrap data")
}
