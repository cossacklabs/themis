package message

import (
    "testing"
    "crypto/rand"
    "math/big"
    "bytes"
    "gothemis/keys"
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
	
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	
	message := make([]byte, int(message_length.Int64()))
	_, err = rand.Read(message)
	if nil != err {
		t.Error(err)
	}
	
	sma := &SecureMessage{kpa.Private, kpb.Public}
	wrapped, err := sma.Wrap(message)
	if nil != err {
		t.Error(err)
	}
	
	if 0 == bytes.Compare(message, wrapped) {
		t.Error("Original message and wrapped message match")
	} 
	
	smb := &SecureMessage{kpb.Private, kpa.Public}
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
	
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
	}
	
	message := make([]byte, int(message_length.Int64()))
	
	sma := &SecureMessage{kp.Private, nil}
	signed, err := sma.Sign(message)
	if nil != err {
		t.Error(err)
	}
	
	if 0 == bytes.Compare(message, signed) {
		t.Error("Original message and signed message match")
	} 
	
	smb := &SecureMessage{nil, kp.Public}
	verified, err := smb.Verify(signed)
	if nil != err {
		t.Error(err)
	}
	
	if 0 != bytes.Compare(message, verified) {
		t.Error("Original message and verified message do not match")
	} 
}

func TestMessageWrap(t *testing.T) {
	testWrap(keys.KEYTYPE_EC, t)
	testWrap(keys.KEYTYPE_RSA, t)
}

func TestMessageSign(t *testing.T) {
	testSign(keys.KEYTYPE_EC, t)
	testSign(keys.KEYTYPE_RSA, t)
}