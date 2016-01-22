package gothemis

import (
    "testing"
    "crypto/rand"
    "math/big"
    "bytes"
)

type testCallbacks struct {
	a *Keypair
	b *Keypair
}

func (clb *testCallbacks) GetPublicKeyForId(ss *SecureSession, id []byte) (*PublicKey) {
	switch {
		case 1 == id[0]:
			return clb.a.public
		case 2 == id[0]:
			return clb.b.public	
	}
	
	return nil
}

func (clb *testCallbacks) StateChanged(ss *SecureSession, state int) {
	
}

/*func genRandData() ([]byte, err) {
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
		return
	}
	
	message := make([]byte, int(message_length.Int64()))
	_, err = rand.Read(message)
	if nil != err {
		t.Error(err)
		return
	}
}

func clientService(client *SecureSession, ch chan []byte, t *testing.T) {
	conReq, err := client.ConnectRequest()
	if nil != err {
		t.Error(err)
		return
	}
	
	ch <- conReq
	for {
		buf := <-ch
		
		buf, sendPeer, err := client.Unwrap(buf)
		if nil != err {
			t.Error(err)
			return
		}
		
		if sendPeer {
			ch <- buf
			continue
		}
		
		
	}
}*/

func testSession(keytype int, t *testing.T) {
	kpa, err := NewKeypair(keytype)
	if nil != err {
		t.Error(err)
		return
	}
	
	kpb, err := NewKeypair(keytype)
	if nil != err {
		t.Error(err)
		return
	}
	
	clb := &testCallbacks{kpa, kpb}
	
	ida := make([]byte, 1)
	ida[0] = 1
	
	idb := make([]byte, 1)
	idb[0] = 2
	
	client, err := NewSession(ida, kpa.private, clb)
	if nil != err {
		t.Error(err)
		return
	}
	
	server, err := NewSession(idb, kpb.private, clb)
	if nil != err {
		t.Error(err)
		return
	}
	
	conReq, err := client.ConnectRequest()
	if nil != err {
		t.Error(err)
		return
	}
	
	buf, sendPeer, err := server.Unwrap(conReq)
	if nil != err {
		t.Error(err)
		return
	}
	if !sendPeer {
		t.Error("Should indicate to send output to peer")
		return
	}
	
	buf, sendPeer, err = client.Unwrap(buf)
	if nil != err {
		t.Error(err)
		return
	}
	if !sendPeer {
		t.Error("Should indicate to send output to peer")
		return
	}
	
	buf, sendPeer, err = server.Unwrap(buf)
	if nil != err {
		t.Error(err)
		return
	}
	if !sendPeer {
		t.Error("Should indicate to send output to peer")
		return
	}
	
	buf, sendPeer, err = client.Unwrap(buf)
	if nil != err {
		t.Error(err)
		return
	}
	if sendPeer {
		t.Error("Should not indicate to send output to peer")
		return
	}
	
	message_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		t.Error(err)
		return
	}
	
	message := make([]byte, int(message_length.Int64()))
	_, err = rand.Read(message)
	if nil != err {
		t.Error(err)
		return
	}
	
	wrapped, err := client.Wrap(message)
	if nil != err {
		t.Error(err)
		return
	}
	
	if 0 == bytes.Compare(message, wrapped) {
		t.Error("Wrapped message and original message match")
		return
	}
	
	unwrapped, sendPeer, err := server.Unwrap(wrapped)
	if nil != err {
		t.Error(err)
		return
	}
	if sendPeer {
		t.Error("Should not indicate to send output to peer")
		return
	}
	
	if 0 != bytes.Compare(message, unwrapped) {
		t.Error("Unwrapped message and original message do not match")
		return
	}
}

func TestSession(t *testing.T) {
	testSession(KEYTYPE_EC, t)
}

