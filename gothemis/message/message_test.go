package message

import (
	"bytes"
	"crypto/rand"
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
