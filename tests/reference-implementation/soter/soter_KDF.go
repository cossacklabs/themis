package soter

/*
#cgo LDFLAGS: -lsoter
#include <stdlib.h>
#include <soter/soter_kdf.h>
*/
import "C"
import "unsafe"

// GoThemis does not export Soter KDF routine. Pull it from Soter directly.

func RealSoterKDF(input []byte, label string, outputBytes int, context ...[]byte) []byte {
	var inputPtr unsafe.Pointer
	var inputLen C.size_t
	if len(input) > 0 {
		inputPtr = unsafe.Pointer(&input[0])
		inputLen = C.size_t(len(input))
	}

	// Go strings are not required to be nil-terminated, but C API wants it.
	labelCStr := C.CString(label)
	defer C.free(unsafe.Pointer(labelCStr))

	// We also need to repackage Go context buffers into whatever C API wants.
	// CGo does not allow to pass Go pointers to Go pointers. We have to copy
	// context buffers into C heap, then free them when done.
	contextArray := make([]C.soter_kdf_context_buf_t, len(context))
	for i, context := range context {
		contextPtr := C.CBytes(context)
		defer C.free(contextPtr)
		contextArray[i].data = (*C.uchar)(contextPtr)
		contextArray[i].length = C.size_t(len(context))
	}
	var contextPtr *C.soter_kdf_context_buf_t
	var contextLen C.size_t
	if len(contextArray) > 0 {
		contextPtr = (*C.soter_kdf_context_buf_t)(unsafe.Pointer(&contextArray[0]))
		contextLen = C.size_t(len(contextArray))
	}

	output := make([]byte, outputBytes)
	outputPtr := unsafe.Pointer(&output[0])
	outputLen := C.size_t(len(output))
	err := C.soter_kdf(inputPtr, inputLen, labelCStr, contextPtr, contextLen, outputPtr, outputLen)
	if err != 0 {
		panic(err)
	}
	return output
}
