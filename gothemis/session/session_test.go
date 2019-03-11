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

var clientId = []byte("client a")
var serverId = []byte("client b")

func (clb *testCallbacks) GetPublicKeyForId(ss *SecureSession, id []byte) *keys.PublicKey {
	if bytes.Equal(clientId, id){
		return clb.a.Public
	} else if bytes.Equal(serverId, id){
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

		remoteId, err := client.GetRemoteId()
		if err != nil{
			t.Error(err)
			return
		}
		if !bytes.Equal(remoteId, serverId){
			t.Error("incorrect remote id")
			return
		}

		if sendPeer {
			ch <- buf
			continue
		}

		if client.GetState() != STATE_ESTABLISHED {
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
		remoteId, err := server.GetRemoteId()
		if err != nil{
			t.Error(err)
			return
		}
		if !bytes.Equal(remoteId, clientId){
			t.Error("incorrect remote id")
			return
		}

		if !sendPeer {
			if server.GetState() != STATE_ESTABLISHED {
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

	emptyKey := keys.PrivateKey{[]byte{}}

	client, err := New(clientId, nil, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty private key")
		return
	}

	client, err = New(clientId, &emptyKey, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty private key")
		return
	}

	client, err = New(nil, kpa.Private, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty id")
		return
	}

	client, err = New([]byte{}, kpa.Private, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty id")
		return
	}

	client, err = New(clientId, kpa.Private, clb)
	if nil != err {
		t.Error(err)
		return
	}

	server, err := New(serverId, kpb.Private, clb)
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

func TestSession(t *testing.T) {
	testSession(keys.KEYTYPE_EC, t)
}
