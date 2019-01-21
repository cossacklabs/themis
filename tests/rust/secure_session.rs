// Copyright 2018 (c) rust-themis developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use std::collections::BTreeMap;
use std::rc::Rc;
use std::sync::mpsc::{channel, Receiver, Sender};

use themis::keygen::gen_ec_key_pair;
use themis::keys::EcdsaPublicKey;
use themis::secure_session::{SecureSession, SecureSessionTransport};

struct DummyTransport {
    key_map: Rc<BTreeMap<Vec<u8>, EcdsaPublicKey>>,
}

impl DummyTransport {
    fn new(key_map: &Rc<BTreeMap<Vec<u8>, EcdsaPublicKey>>) -> Self {
        Self {
            key_map: key_map.clone(),
        }
    }
}

impl SecureSessionTransport for DummyTransport {
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
        self.key_map.get(id).cloned()
    }
}

struct ChannelTransport {
    key_map: Rc<BTreeMap<Vec<u8>, EcdsaPublicKey>>,
    tx: Sender<Vec<u8>>,
    rx: Receiver<Vec<u8>>,
}

impl ChannelTransport {
    #[allow(clippy::new_ret_no_self)]
    fn new(key_map: &Rc<BTreeMap<Vec<u8>, EcdsaPublicKey>>) -> (Self, Self) {
        let (tx12, rx21) = channel();
        let (tx21, rx12) = channel();

        let transport1 = Self {
            key_map: key_map.clone(),
            tx: tx12,
            rx: rx12,
        };
        let transport2 = Self {
            key_map: key_map.clone(),
            tx: tx21,
            rx: rx21,
        };

        (transport1, transport2)
    }
}

impl SecureSessionTransport for ChannelTransport {
    fn send_data(&mut self, data: &[u8]) -> Result<usize, ()> {
        self.tx
            .send(data.to_vec())
            .map(|_| data.len())
            .map_err(|_| ())
    }

    fn receive_data(&mut self, data: &mut [u8]) -> Result<usize, ()> {
        let msg = self.rx.recv().map_err(|_| ())?;
        if msg.len() > data.len() {
            return Err(());
        }
        data[0..msg.len()].copy_from_slice(&msg);
        Ok(msg.len())
    }

    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
        self.key_map.get(id).cloned()
    }
}

#[test]
fn no_transport() {
    // Peer credentials. Secure Session supports only ECDSA.
    // TODO: tests that confirm RSA failure
    let (secret_client, public_client) = gen_ec_key_pair().split();
    let (secret_server, public_server) = gen_ec_key_pair().split();
    let (name_client, name_server) = ("client", "server");

    // Shared storage of public peer credentials. These should be communicated between
    // the peers beforehand in some unspecified trusted way.
    let mut key_map = BTreeMap::new();
    key_map.insert(name_client.as_bytes().to_vec(), public_client);
    key_map.insert(name_server.as_bytes().to_vec(), public_server);
    let key_map = Rc::new(key_map);

    // The client and the server.
    let mut client =
        SecureSession::with_transport(name_client, &secret_client, DummyTransport::new(&key_map))
            .unwrap();
    let mut server =
        SecureSession::with_transport(name_server, &secret_server, DummyTransport::new(&key_map))
            .unwrap();

    assert!(!client.is_established());
    assert!(!server.is_established());
    assert!(client.get_remote_id().unwrap().is_empty());
    assert!(server.get_remote_id().unwrap().is_empty());

    // Connection and key negotiation sequence.
    let connect_request = client.generate_connect_request().expect("connect request");
    let connect_reply = server.negotiate(&connect_request).expect("connect reply");
    let key_proposed = client.negotiate(&connect_reply).expect("key proposed");
    let key_accepted = server.negotiate(&key_proposed).expect("key accepted");
    let key_confirmed = client.negotiate(&key_accepted).expect("key confirmed");
    assert!(key_confirmed.is_empty());

    assert!(client.is_established());
    assert!(server.is_established());
    assert_eq!(client.get_remote_id().unwrap(), name_server.as_bytes());
    assert_eq!(server.get_remote_id().unwrap(), name_client.as_bytes());

    // TODO: check connection states reported to transport delegate

    // Try sending a message back and forth.
    let plaintext = b"test message please ignore";

    let wrapped = client.wrap(&plaintext).expect("wrap 1 -> 2 message");
    let unwrapped = server.unwrap(&wrapped).expect("unwrap 1 -> 2 message");
    assert_eq!(unwrapped, plaintext);

    let wrapped = server.wrap(&plaintext).expect("wrap 2 -> 1 message");
    let unwrapped = client.unwrap(&wrapped).expect("unwrap 2 -> 1 message");
    assert_eq!(unwrapped, plaintext);

    // TODO: it seems that one cannot wrap an empty message, check it out

    // Messages are independent, can come out-of-order and be lost.
    client.wrap(b"some message").expect("lost message 1");
    client.wrap(b"some message").expect("lost message 2");
    server.wrap(b"some message").expect("lost message 3");

    let wrapped1 = client.wrap(b"message 1").expect("message 1");
    let wrapped2 = client.wrap(b"message 2").expect("message 2");
    let unwrapped2 = server.unwrap(&wrapped2).expect("message 2");
    let unwrapped1 = server.unwrap(&wrapped1).expect("message 1");
    assert_eq!(unwrapped1, b"message 1");
    assert_eq!(unwrapped2, b"message 2");
}

#[test]
fn with_transport() {
    // Peer credentials. Secure Session supports only ECDSA.
    // TODO: tests that confirm RSA failure
    let (secret_client, public_client) = gen_ec_key_pair().split();
    let (secret_server, public_server) = gen_ec_key_pair().split();
    let (name_client, name_server) = ("client", "server");

    // Shared storage of public peer credentials. These should be communicated between
    // the peers beforehand in some unspecified trusted way.
    let mut key_map = BTreeMap::new();
    key_map.insert(name_client.as_bytes().to_vec(), public_client);
    key_map.insert(name_server.as_bytes().to_vec(), public_server);
    let key_map = Rc::new(key_map);

    // The client and the server.
    let (transport_client, transport_server) = ChannelTransport::new(&key_map);
    let mut client =
        SecureSession::with_transport(name_client, &secret_client, transport_client).unwrap();
    let mut server =
        SecureSession::with_transport(name_server, &secret_server, transport_server).unwrap();

    assert!(!client.is_established());
    assert!(!server.is_established());

    // Establishing connection.
    client.connect().expect("client-side connection");
    server.negotiate_transport().expect("connect reply");
    client.negotiate_transport().expect("key proposed");
    server.negotiate_transport().expect("key accepted");
    client.negotiate_transport().expect("key confirmed");

    assert!(client.is_established());
    assert!(server.is_established());

    // Try sending a message back and forth.
    let message = b"test message please ignore";
    client.send(&message).expect("send message");

    let received = server.receive(1024).expect("receive message");

    assert_eq!(received, message);
}
