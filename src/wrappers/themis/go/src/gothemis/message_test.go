package gothemis

import (
    "testing"
    "crypto/rand"
    "math/big"
    "bytes"
)

func testWrap(keytype int, t *testing.T) {
	kpa, err := NewKeypair(keytype)
	if nil != err {
		t.Error(err)
	}
	
	kpb, err := NewKeypair(keytype)
	if nil != err {
		t.Error(err)
	}
	
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	
	message := make([]byte, int(message_length.Int64()))
	
	sma := &SecureMessage{kpa.private, kpb.public}
	wrapped, err := sma.Wrap(message)
	if nil != err {
		t.Error(err)
	}
	
	if 0 == bytes.Compare(message, wrapped) {
		t.Error("Original message and wrapped message match")
	} 
	
	smb := &SecureMessage{kpb.private, kpa.public}
	unwrapped, err := smb.Unwrap(wrapped)
	if nil != err {
		t.Error(err)
	}
	
	if 0 != bytes.Compare(message, unwrapped) {
		t.Error("Original message and unwrapped message do not match")
	} 
}

func testSign(keytype int, t *testing.T) {
	kp, err := NewKeypair(keytype)
	if nil != err {
		t.Error(err)
	}
	
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	
	message := make([]byte, int(message_length.Int64()))
	
	sma := &SecureMessage{kp.private, nil}
	signed, err := sma.Sign(message)
	if nil != err {
		t.Error(err)
	}
	
	if 0 == bytes.Compare(message, signed) {
		t.Error("Original message and signed message match")
	} 
	
	smb := &SecureMessage{nil, kp.public}
	verified, err := smb.Verify(signed)
	if nil != err {
		t.Error(err)
	}
	
	if 0 != bytes.Compare(message, verified) {
		t.Error("Original message and verified message do not match")
	} 
}

func TestMessageWrap(t *testing.T) {
	testWrap(KEYTYPE_EC, t)
	testWrap(KEYTYPE_RSA, t)
}

func TestMessageSign(t *testing.T) {
	testSign(KEYTYPE_EC, t)
	testSign(KEYTYPE_RSA, t)
}