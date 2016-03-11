package gothemis

/*
#cgo LDFLAGS: -lthemis -lsoter
#include "session.h"
*/
import "C"
import (
	"unsafe"
	"errors"
	"runtime"
)

const (
	STATE_IDLE = 0
	STATE_NEGOTIATING = 1
	STATE_ESTABLISHED = 2
)

type SessionCallbacks interface {
	GetPublicKeyForId(ss *SecureSession, id []byte) (*PublicKey)
	StateChanged(ss *SecureSession, state int)
}

type SecureSession struct {
	ctx []byte
	clb SessionCallbacks
	state int
}

func finalizeSession(ss *SecureSession) {
	ss.Close()
}

func NewSession(id []byte, signKey *PrivateKey, callbacks SessionCallbacks) (*SecureSession, error) {
	ctx := make([]byte, C.get_session_ctx_size())
	ss := &SecureSession{ctx: ctx, clb: callbacks}
	
	if ! bool(C.session_init(unsafe.Pointer(ss),
			(*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ctx[0])),
			unsafe.Pointer(&id[0]),
			C.size_t(len(id)),
			unsafe.Pointer(&signKey.value[0]),
			C.size_t(len(signKey.value)))) {
				return nil, errors.New("Failed to create secure session object");
			}
			
	runtime.SetFinalizer(ss, finalizeSession)
	
	return ss, nil
}

func (ss *SecureSession) Close() error {
	if nil != ss.ctx {
		if bool(C.session_destroy((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])))) {
			ss.ctx = nil
		} else {
			return errors.New("Failed to destroy secure session object")
		}
	}
	
	return nil
}

//export onPublicKeyForId
func onPublicKeyForId(ssCtx unsafe.Pointer, idPtr unsafe.Pointer, idLen C.size_t, keyPtr unsafe.Pointer, keyLen C.size_t) (int) {
	id := C.GoBytes(idPtr, C.int(idLen))
	ss := (*SecureSession)(ssCtx)
	
	pub := ss.clb.GetPublicKeyForId(ss, id)
	if nil == pub {
		return -2 // THEMIS_INVALID_PARAMETER
	}
	
	if len(pub.value) > int(keyLen) {
		return -4 // THEMIS_BUFFER_TOO_SMALL
	}
	
	key := (*[1 << 30]byte)(keyPtr)[:keyLen:keyLen]
	copy(key, pub.value)
	return 0
}

//export onStateChanged
func onStateChanged(ssCtx unsafe.Pointer, event int) {
	ss := (*SecureSession)(ssCtx)
	
	ss.clb.StateChanged(ss, event)
}

func (ss *SecureSession) ConnectRequest() ([]byte, error) {
	var reqLen C.size_t
	
	if ! bool(C.session_connect_size((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
			&reqLen)) {
				return nil, errors.New("Failed to get request size");
			}
	
	req := make([]byte, reqLen)
	if ! bool(C.session_connect((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
			unsafe.Pointer(&req[0]),
			reqLen)) {
				return nil, errors.New("Failed to generate request");
			}
	
	return req, nil
}

func (ss *SecureSession) Wrap(data []byte) ([]byte, error) {
	var outLen C.size_t
	
	if ! bool(C.session_wrap_size((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
			unsafe.Pointer(&data[0]),
			C.size_t(len(data)),
			&outLen)) {
				return nil, errors.New("Failed to get wrapped size");
			}
	
	out := make([]byte, outLen)
	if ! bool(C.session_wrap((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
			unsafe.Pointer(&data[0]),
			C.size_t(len(data)),
			unsafe.Pointer(&out[0]),
			outLen)) {
				return nil, errors.New("Failed to wrap data");
			}
	
	return out, nil
}

func (ss *SecureSession) Unwrap(data []byte) ([]byte, bool, error) {
	var outLen C.size_t
	
	res := C.session_unwrap_size((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
		unsafe.Pointer(&data[0]),
		C.size_t(len(data)),
		&outLen)
	
	switch {
		case (0 == res) && (0 == outLen):
			return nil, false, nil
		case (-4 != res): // THEMIS_BUFFER_TOO_SMALL
			return nil, false, errors.New("Failed to get unwrapped size")
	}
	
	out := make([]byte, outLen)
	
	res = C.session_unwrap((*C.struct_session_with_callbacks_type)(unsafe.Pointer(&ss.ctx[0])),
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
	}
	
	return nil, false, errors.New("Failed to unwrap data")
}