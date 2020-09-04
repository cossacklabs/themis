package compare

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/themis_error.h>
#include <themis/secure_comparator.h>

static void* compare_init(void)
{
	return secure_comparator_create();
}

static bool compare_destroy(void *ctx)
{
	return THEMIS_SUCCESS == secure_comparator_destroy(ctx);
}

static bool compare_append(void *ctx, const void *secret, size_t secret_length)
{
	return THEMIS_SUCCESS == secure_comparator_append_secret(ctx, secret, secret_length);
}

static bool compare_begin_size(void *ctx, size_t *out_len)
{
	return THEMIS_BUFFER_TOO_SMALL == secure_comparator_begin_compare(ctx, NULL, out_len);
}

static bool compare_begin(void *ctx, void *out, size_t out_len)
{
	return THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER == secure_comparator_begin_compare(ctx, out, &out_len);
}

static bool compare_proceed_size(void *ctx, const void *in, size_t in_len, size_t *out_len)
{
	return THEMIS_BUFFER_TOO_SMALL == secure_comparator_proceed_compare(ctx, in, in_len, NULL, out_len);
}

static int compare_proceed(void *ctx, const void *in, size_t in_len, void *out, size_t out_len)
{
	return secure_comparator_proceed_compare(ctx, in, in_len, out, &out_len);
}

static int compare_result(void *ctx)
{
	themis_status_t res = secure_comparator_get_result(ctx);
	return (int)res;
}

*/
import "C"
import (
	"runtime"
	"unsafe"

	"github.com/cossacklabs/themis/gothemis/errors"
)

// Secure comparison result.
const (
	Match    = int(C.THEMIS_SCOMPARE_MATCH)
	NoMatch  = int(C.THEMIS_SCOMPARE_NO_MATCH)
	NotReady = int(C.THEMIS_SCOMPARE_NOT_READY)
)

// Secure comparison result.
//
// Deprecated: Since 0.11. Use "compare.Match..." constants instead.
const (
	COMPARE_MATCH     = Match
	COMPARE_NO_MATCH  = NoMatch
	COMPARE_NOT_READY = NotReady
)

// Errors returned by Secure Comparator.
var (
	ErrAppendSecret             = errors.New("failed to append secret")
	ErrCreateComparator         = errors.New("failed to create comparator object")
	ErrDestroyComparator        = errors.New("failed to destroy comparator object")
	ErrProtocolData             = errors.New("failed to get protocol data")
	ErrProtocolDataSize         = errors.New("failed to get protocol data size")
	ErrNoResult                 = errors.New("failed to get result")
	ErrMissingSecret            = errors.NewWithCode(errors.InvalidParameter, "empty secret for Secure Comparator")
	ErrMissingData              = errors.NewWithCode(errors.InvalidParameter, "empty comparison message for Secure Comparator")
	ErrOutOfMemory              = errors.NewWithCode(errors.NoMemory, "Secure Comparator cannot allocate enough memory")
	// Deprecated: Since 0.14. Use ErrOutOfMemory instead.
	ErrOverflow                 = ErrOutOfMemory
)

// SecureCompare is an interactive protocol for two parties that compares whether
// they share the same secret or not.
type SecureCompare struct {
	ctx unsafe.Pointer
}

func finalize(sc *SecureCompare) {
	sc.Close()
}

// C returns sizes as size_t but Go expresses buffer lengths as int.
// Make sure that all sizes are representable in Go and there is no overflows.
func sizeOverflow(n C.size_t) bool {
	const maxInt = int(^uint(0) >> 1)
	return n > C.size_t(maxInt)
}

// New prepares a new secure comparison.
func New() (*SecureCompare, error) {
	ctx := C.compare_init()
	if nil == ctx {
		return nil, ErrCreateComparator
	}

	sc := &SecureCompare{ctx}
	runtime.SetFinalizer(sc, finalize)

	return sc, nil
}

// Close destroys Secure Comparator.
func (sc *SecureCompare) Close() error {
	if nil != sc.ctx {
		if bool(C.compare_destroy(sc.ctx)) {
			sc.ctx = nil
		} else {
			return ErrDestroyComparator
		}
	}

	return nil
}

// Append adds data to be compared.
func (sc *SecureCompare) Append(secret []byte) error {
	if nil == secret || 0 == len(secret) {
		return ErrMissingSecret
	}
	if !bool(C.compare_append(sc.ctx, unsafe.Pointer(&secret[0]), C.size_t(len(secret)))) {
		return ErrAppendSecret
	}

	return nil
}

// Begin initiates secure comparison and returns data to be sent to the peer.
func (sc *SecureCompare) Begin() ([]byte, error) {
	var outLen C.size_t

	if !bool(C.compare_begin_size(sc.ctx, &outLen)) {
		return nil, ErrProtocolDataSize
	}
	if sizeOverflow(outLen) {
		return nil, ErrOutOfMemory
	}

	out := make([]byte, outLen)

	if !bool(C.compare_begin(sc.ctx, unsafe.Pointer(&out[0]), outLen)) {
		return nil, ErrProtocolData
	}

	return out, nil
}

// Proceed continues the comparison process with peer data and returns a reply.
// Comparison is complete when this method returns nil successfully.
func (sc *SecureCompare) Proceed(data []byte) ([]byte, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, ErrMissingData
	}

	if !bool(C.compare_proceed_size(sc.ctx, unsafe.Pointer(&data[0]), C.size_t(len(data)), &outLen)) {
		return nil, ErrProtocolDataSize
	}
	if sizeOverflow(outLen) {
		return nil, ErrOutOfMemory
	}

	if 0 == outLen {
		outLen++
	}

	out := make([]byte, outLen)

	res := C.compare_proceed(sc.ctx, unsafe.Pointer(&data[0]), C.size_t(len(data)), unsafe.Pointer(&out[0]), outLen)
	switch {
	case 0 == res:
		return nil, nil
	case 1 == res:
		return out, nil
	}

	return nil, ErrProtocolData
}

// Result returns the result of the comparison.
func (sc *SecureCompare) Result() (int, error) {
	res := int(C.compare_result(sc.ctx))
	switch res {
	case NotReady, NoMatch, Match:
		return int(res), nil
	}

	return NotReady, ErrNoResult
}
