package message

import (
	"bytes"
	"crypto/rand"
	"encoding/base64"
	"math/big"
	"testing"

	"github.com/cossacklabs/themis/gothemis/keys"
)

func testWrap(keytype int, t *testing.T) {
	kpa, err := keys.New(keytype)
	if nil != err {
		t.Error(err)
	}

	kpb, err := keys.New(keytype)
	if nil != err {
		t.Error(err)
	}

	messageLength, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	length := messageLength.Int64()
	if length == 0 {
		length = 1
	}

	message := make([]byte, length)
	_, err = rand.Read(message)
	if nil != err {
		t.Error(err)
	}

	sma := New(kpa.Private, kpb.Public)

	_, err = sma.Wrap(nil)
	if nil == err {
		t.Error("Secure message empty data encryption")
	}

	_, err = sma.Wrap([]byte{})
	if nil == err {
		t.Error("Secure message empty data encryption")
	}

	wrapped, err := sma.Wrap(message)
	if nil != err {
		t.Error(err)
	}

	if 0 == bytes.Compare(message, wrapped) {
		t.Error("Original message and wrapped message match")
	}

	smb := New(kpb.Private, kpa.Public)

	_, err = smb.Unwrap(nil)
	if nil == err {
		t.Error("Secure Message empty data decryption")
	}

	_, err = smb.Unwrap([]byte{})
	if nil == err {
		t.Error("Secure Message empty data decryption")
	}

	unwrapped, err := smb.Unwrap(wrapped)
	if nil != err {
		t.Error(err)
	}

	if 0 != bytes.Compare(message, unwrapped) {
		t.Error("Original message and unwrapped message do not match")
	}
}

func testSign(keytype int, t *testing.T) {
	kp, err := keys.New(keytype)
	if nil != err {
		t.Error(err)
	}

	messageLength, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	length := messageLength.Int64()
	if length == 0 {
		length = 1
	}

	message := make([]byte, length)

	sma := New(kp.Private, nil)
	signed, err := sma.Sign(message)
	if nil != err {
		t.Error(err)
	}

	if 0 == bytes.Compare(message, signed) {
		t.Error("Original message and signed message match")
	}

	smb := New(nil, kp.Public)
	verified, err := smb.Verify(signed)
	if nil != err {
		t.Error(err)
	}

	if 0 != bytes.Compare(message, verified) {
		t.Error("Original message and verified message do not match")
	}
}

func TestMessageWrap(t *testing.T) {
	testWrap(keys.TypeEC, t)
	testWrap(keys.TypeRSA, t)
}

func TestMessageSign(t *testing.T) {
	testSign(keys.TypeEC, t)
	testSign(keys.TypeRSA, t)
}

func BenchmarkSMessageEncryptRSA(b *testing.B) {
	keyPair, err := keys.New(keys.TypeRSA)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Wrap(plaintext)
		if err != nil {
			b.Errorf("encryption failed: %v", err)
		}
	}
}

func BenchmarkSMessageEncryptEC(b *testing.B) {
	keyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Wrap(plaintext)
		if err != nil {
			b.Errorf("encryption failed: %v", err)
		}
	}
}

func BenchmarkSMessageDecryptRSA(b *testing.B) {
	keyPair, err := keys.New(keys.TypeRSA)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	encrypted, err := smessage.Wrap(plaintext)
	if err != nil {
		b.Errorf("failed to encrypt message: %v", err)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Unwrap(encrypted)
		if err != nil {
			b.Errorf("decryption failed: %v", err)
		}
	}
}

func BenchmarkSMessageDecryptEC(b *testing.B) {
	keyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	encrypted, err := smessage.Wrap(plaintext)
	if err != nil {
		b.Errorf("failed to encrypt message: %v", err)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Unwrap(encrypted)
		if err != nil {
			b.Errorf("decryption failed: %v", err)
		}
	}
}

func BenchmarkSMessageSignRSA(b *testing.B) {
	keyPair, err := keys.New(keys.TypeRSA)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Sign(plaintext)
		if err != nil {
			b.Errorf("signing failed: %v", err)
		}
	}
}

func BenchmarkSMessageSignEC(b *testing.B) {
	keyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Sign(plaintext)
		if err != nil {
			b.Errorf("signing failed: %v", err)
		}
	}
}

func BenchmarkSMessageVerifyRSA(b *testing.B) {
	keyPair, err := keys.New(keys.TypeRSA)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	encrypted, err := smessage.Sign(plaintext)
	if err != nil {
		b.Errorf("failed to sign message: %v", err)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Verify(encrypted)
		if err != nil {
			b.Errorf("verification failed: %v", err)
		}
	}
}

func BenchmarkSMessageVerifyEC(b *testing.B) {
	keyPair, err := keys.New(keys.TypeEC)
	if err != nil {
		b.Errorf("failed to generate key pair: %v", err)
	}
	plaintext := make([]byte, 1024)
	_, err = rand.Read(plaintext)
	if err != nil {
		b.Errorf("failed to generate plaintext: %v", err)
	}
	smessage := New(keyPair.Private, keyPair.Public)
	encrypted, err := smessage.Sign(plaintext)
	if err != nil {
		b.Errorf("failed to sign message: %v", err)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, err := smessage.Verify(encrypted)
		if err != nil {
			b.Errorf("verification failed: %v", err)
		}
	}
}

// Check that GoThemis is still able to verify old signed messages with extra bytes at the end.
// Modern versions of GoThemis do not produce such messages.
func TestVerifyOverlongEllipticSignatures(t *testing.T) {
	publicKey := &keys.PublicKey{
		Value: decodeString("VUVDMgAAAC2EoQKyAzIj/ESwMOntizF5o+aGtG/a3rd38HE3Hq9YxwS6bIIQ"),
	}
	signatures := []string{
		// NIST P-256 curve, 69..72 bytes long signatures, with and without zero padding
		"ICYEJk8AAABIAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBGAiEA2RTru1XIEjVuOVZhWIiU68gCGS1vLIavK2/h8Tp35yMCIQD2tPLZ+7GTPrIgHvFPi82iH7ghKESlsbFwNRSu4QVapA==",
		"ICYEJk8AAABHAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBFAiBsaS6CcXhMuVNBkMLh94wh6MzrCGf/iv5YAbzwsasUcgIhAKm/jWtvxfRdL7gUxr3yUU08gfd0ZQhFUOipoOEJcvXDAA==",
		"ICYEJk8AAABHAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBFAiBsaS6CcXhMuVNBkMLh94wh6MzrCGf/iv5YAbzwsasUcgIhAKm/jWtvxfRdL7gUxr3yUU08gfd0ZQhFUOipoOEJcvXD",
		"ICYEJk8AAABGAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBEAiBMDtaLonRC9wzc4yj4C/5zHJSFlxnWJ1BS279WpbshzgIgRmXEoGyefv/W4Jl5TitjKQrRbpWb1RI6jgnyti3kh3wAAA==",
		"ICYEJk8AAABGAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBEAiBMDtaLonRC9wzc4yj4C/5zHJSFlxnWJ1BS279WpbshzgIgRmXEoGyefv/W4Jl5TitjKQrRbpWb1RI6jgnyti3kh3w=",
		"ICYEJk8AAABFAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBDAiAgM9Vi17JZdp7B5QXu8Jge3uZNp2Ry99g8F6VRJtdw5QIfPgJTFaIJCz7Iv8oyly1eCkhQixLHUowBCyyf0idFkAAAAA==",
		"ICYEJk8AAABFAAAAQm9sw6lybyBpcyBhIG9uZS1tb3ZlbWVudCBvcmNoZXN0cmFsIHBpZWNlIGJ5IHRoZSBGcmVuY2ggY29tcG9zZXIgTWF1cmljZSBSYXZlbDBDAiAgM9Vi17JZdp7B5QXu8Jge3uZNp2Ry99g8F6VRJtdw5QIfPgJTFaIJCz7Iv8oyly1eCkhQixLHUowBCyyf0idFkA==",
	}
	expected := "Boléro is a one-movement orchestral piece by the French composer Maurice Ravel"

	smessage := New(nil, publicKey)
	for i, signature := range signatures {
		signature := decodeString(signature)
		verified, err := smessage.Verify(signature)
		if err != nil {
			t.Errorf("failed to verify signature[%d] of length %d: %v", i, len(signature), err)
		}
		if string(verified) != expected {
			t.Logf("signature[%d] of length %d", i, len(signature))
			t.Logf("expected: %s", expected)
			t.Logf("actual:   %s", string(verified))
			t.Error("verified messages do not match")
		}
	}
}

func decodeString(str string) []byte {
	bytes, err := base64.StdEncoding.DecodeString(str)
	if err != nil {
		panic(err)
	}
	return bytes
}
