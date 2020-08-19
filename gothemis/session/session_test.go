package session

import (
	"bytes"
	"crypto/rand"
	"errors"
	"github.com/cossacklabs/themis/gothemis/keys"
	"math/big"
	"testing"
)

type testCallbacks struct {
	a *keys.Keypair
	b *keys.Keypair
}

var clientID = []byte("client a")
var serverID = []byte("client b")

func (clb *testCallbacks) GetPublicKeyForId(ss *SecureSession, id []byte) *keys.PublicKey {
	if bytes.Equal(clientID, id) {
		return clb.a.Public
	} else if bytes.Equal(serverID, id) {
		return clb.b.Public
	}
	return nil
}

func (clb *testCallbacks) StateChanged(ss *SecureSession, state int) {

}

func genRandData() ([]byte, error) {
	dataLength, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		return nil, err
	}
	length := dataLength.Int64()
	if length == 0 {
		length = 1
	}
	data := make([]byte, length)
	_, err = rand.Read(data)
	if nil != err {
		return nil, err
	}

	return data, nil
}

func fin() []byte {
	f := [4]byte{0xDE, 0xAD, 0xC0, 0xDE}
	return f[:]
}

func isFin(b []byte) bool {
	return 0 == bytes.Compare(b, fin())
}

func clientService(client *SecureSession, ch chan []byte, finCh chan int, t *testing.T) {
	defer func() {
		if t.Failed() {
			finCh <- 0
		}
	}()

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

		remoteID, err := client.GetRemoteID()
		if err != nil {
			t.Error(err)
			return
		}
		if !bytes.Equal(remoteID, serverID) {
			t.Error("incorrect remote id")
			return
		}

		if sendPeer {
			ch <- buf
			continue
		}

		if client.GetState() != StateEstablished {
			t.Error(errors.New("Incorrect secure session state"))
		}

		var finish bool
		if nil == buf {
			buf, _ = genRandData()
		} else {
			buf = fin()
			finish = true
		}

		buf, err = client.Wrap(buf)
		if nil != err {
			t.Error(err)
			return
		}
		ch <- buf

		if finish {
			break
		}
	}
}

func serverService(server *SecureSession, ch chan []byte, finCh chan int, t *testing.T) {
	defer func() {
		finCh <- 0
	}()

	for {
		buf := <-ch

		buf, sendPeer, err := server.Unwrap(buf)
		if nil != err {
			t.Error(err)
			return
		}
		remoteID, err := server.GetRemoteID()
		if err != nil {
			t.Error(err)
			return
		}
		if !bytes.Equal(remoteID, clientID) {
			t.Error("incorrect remote id")
			return
		}

		if !sendPeer {
			if server.GetState() != StateEstablished {
				t.Error(errors.New("Incorrect secure session state"))
			}
			if isFin(buf) {
				break
			}

			buf, err = server.Wrap(buf)
			if nil != err {
				t.Error(err)
				return
			}
		}

		ch <- buf
	}
}

func testSession(keytype int, t *testing.T) {
	kpa, err := keys.New(keytype)
	if nil != err {
		t.Error(err)
		return
	}

	kpb, err := keys.New(keytype)
	if nil != err {
		t.Error(err)
		return
	}

	clb := &testCallbacks{kpa, kpb}

	emptyKey := keys.PrivateKey{Value: []byte{}}

	_, err = New(clientID, nil, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty private key")
		return
	}

	_, err = New(clientID, &emptyKey, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty private key")
		return
	}

	_, err = New(nil, kpa.Private, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty id")
		return
	}

	_, err = New([]byte{}, kpa.Private, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty id")
		return
	}

	client, err := New(clientID, kpa.Private, clb)
	if nil != err {
		t.Error(err)
		return
	}

	server, err := New(serverID, kpb.Private, clb)
	if nil != err {
		t.Error(err)
		return
	}

	ch := make(chan []byte)
	finCh := make(chan int)
	go serverService(server, ch, finCh, t)
	go clientService(client, ch, finCh, t)

	<-finCh
}

func TestValidKeyHandling(t *testing.T) {
	kpa, err := keys.New(keys.TypeEC)
	if err != nil {
		t.Error(err)
		return
	}

	kpb, err := keys.New(keys.TypeEC)
	if err != nil {
		t.Error(err)
		return
	}

	clb := &testCallbacks{kpa, kpb}

	client, err := New(clientID, kpa.Private, clb)
	if client == nil || err != nil {
		t.Error("Creating Secure Session with EC key", err)
		return
	}
}

func TestInvalidKeyHandling(t *testing.T) {
	kpa, err := keys.New(keys.TypeRSA)
	if err != nil {
		t.Error(err)
		return
	}

	kpb, err := keys.New(keys.TypeRSA)
	if err != nil {
		t.Error(err)
		return
	}

	clb := &testCallbacks{kpa, kpb}

	client, err := New(clientID, kpa.Private, clb)
	if client != nil || err == nil {
		t.Error("Creating Secure Session with RSA key")
		return
	}
}

func TestSession(t *testing.T) {
	testSession(keys.TypeEC, t)
}
