package compare

import (
	"crypto/rand"
	"math/big"
	"testing"
)

func genRandData() ([]byte, error) {
	dataLength, err := rand.Int(rand.Reader, big.NewInt(256))
	if nil != err {
		return nil, err
	}
	size := dataLength.Int64()
	if size == 0 {
		size = 1
	}

	data := make([]byte, size)
	_, err = rand.Read(data)
	if nil != err {
		return nil, err
	}

	return data, nil
}

func scService(sc *SecureCompare, ch chan []byte, finCh chan int, t *testing.T) {
	res, err := sc.Result()
	if err != nil {
		t.Error(err)
		finCh <- NotReady
		return
	}

	for NotReady == res {
		buf := <-ch

		buf, err := sc.Proceed(buf)
		if err != nil {
			t.Error(err)
			finCh <- NotReady
			return
		}

		if nil != buf {
			ch <- buf
		}

		res, err = sc.Result()
		if err != nil {
			t.Error(err)
			finCh <- NotReady
			return
		}
	}

	finCh <- res
}

func compare(sec1, sec2 []byte, expected int, t *testing.T) {
	alice, err := New()
	if err != nil {
		t.Error(err)
		return
	}

	bob, err := New()
	if err != nil {
		t.Error(err)
		return
	}

	err = alice.Append(sec1)
	if err != nil {
		t.Error(err)
		return
	}

	err = bob.Append(sec2)
	if err != nil {
		t.Error(err)
		return
	}

	start, err := alice.Begin()
	if err != nil {
		t.Error(err)
		return
	}

	ch := make(chan []byte)
	finCh := make(chan int)

	go scService(bob, ch, finCh, t)
	ch <- start
	go scService(alice, ch, finCh, t)

	res1 := <-finCh
	res2 := <-finCh

	if res1 != res2 {
		t.Errorf("Results do not match: %d %d", res1, res2)
		return
	}

	if res1 != expected {
		t.Errorf("Incorrect match result")
		return
	}
}

func TestCompare(t *testing.T) {
	sec1, err := genRandData()
	if err != nil {
		t.Error(err)
		return
	}

	sec2, err := genRandData()
	if err != nil {
		t.Error(err)
		return
	}

	compare(sec1, sec2, NoMatch, t)
	compare(sec1, sec1, Match, t)
}
