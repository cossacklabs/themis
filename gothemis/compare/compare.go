package compare

/*
#cgo LDFLAGS: -lthemis -lsoter
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <themis/error.h>
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

	if (THEMIS_SCOMPARE_MATCH == res)
	{
		return 1;
	}

	return (int)res;
}

*/
import "C"
import (
	"github.com/cossacklabs/themis/gothemis/errors"
	"runtime"
	"unsafe"
)

const (
	COMPARE_NOT_READY = 0
	COMPARE_NO_MATCH  = -1
	COMPARE_MATCH     = 1
)

type SecureCompare struct {
	ctx unsafe.Pointer
}

func finalize(sc *SecureCompare) {
	sc.Close()
}

func New() (*SecureCompare, error) {
	ctx := C.compare_init()
	if nil == ctx {
		return nil, errors.New("Failed to create comparator object")
	}

	sc := &SecureCompare{ctx}
	runtime.SetFinalizer(sc, finalize)

	return sc, nil
}

func (sc *SecureCompare) Close() error {
	if nil != sc.ctx {
		if bool(C.compare_destroy(sc.ctx)) {
			sc.ctx = nil
		} else {
			return errors.New("Failed to destroy comparator object")
		}
	}

	return nil
}

func (sc *SecureCompare) Append(secret []byte) error {
	if nil == secret || 0 == len(secret) {
		return errors.New("Secret was not provided")
	}
	if !bool(C.compare_append(sc.ctx, unsafe.Pointer(&secret[0]), C.size_t(len(secret)))) {
		return errors.New("Failed to append secret")
	}

	return nil
}

func (sc *SecureCompare) Begin() ([]byte, error) {
	var outLen C.size_t

	if !bool(C.compare_begin_size(sc.ctx, &outLen)) {
		return nil, errors.New("Failed to get output size")
	}

	out := make([]byte, outLen)

	if !bool(C.compare_begin(sc.ctx, unsafe.Pointer(&out[0]), outLen)) {
		return nil, errors.New("Failed to get compare data")
	}

	return out, nil
}

func (sc *SecureCompare) Proceed(data []byte) ([]byte, error) {
	var outLen C.size_t

	if nil == data || 0 == len(data) {
		return nil, errors.New("Data was not provided")
	}

	if !bool(C.compare_proceed_size(sc.ctx, unsafe.Pointer(&data[0]), C.size_t(len(data)), &outLen)) {
		return nil, errors.New("Failed to get output size")
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

	return nil, errors.New("Failed to get output")
}

func (sc *SecureCompare) Result() (int, error) {
	res := C.compare_result(sc.ctx)

	switch res {
	case COMPARE_NOT_READY, COMPARE_NO_MATCH, COMPARE_MATCH:
		return int(res), nil
	}

	return COMPARE_NOT_READY, errors.New("Failed to get compare result")
}
