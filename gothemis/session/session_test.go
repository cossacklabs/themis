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

func (clb *testCallbacks) GetPublicKeyForId(ss *SecureSession, id []byte) *keys.PublicKey {
	switch {
	case 1 == id[0]:
		return clb.a.Public
	case 2 == id[0]:
		return clb.b.Public
	}

	return nil
}

func (clb *testCallbacks) StateChanged(ss *SecureSession, state int) {

}

func genRandData() ([]byte, error) {
	data_length, err := rand.Int(rand.Reader, big.NewInt(2048))
	if nil != err {
		return nil, err
	}

	data := make([]byte, int(data_length.Int64()))
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

	ida := make([]byte, 1)
	ida[0] = 1

	idb := make([]byte, 1)
	idb[0] = 2

	empty_key := keys.PrivateKey{[]byte{}}

	client, err := New(ida, nil, clb)
	if nil == err {
		t.Error("Creating Secure session object with empty private key")
		return
	}

	client, err = New(ida, &empty_key, clb)
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

	client, err = New(ida, kpa.Private, clb)
	if nil != err {
		t.Error(err)
		return
	}

	server, err := New(idb, kpb.Private, clb)
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
