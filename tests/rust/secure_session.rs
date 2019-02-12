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

use std::sync::mpsc::{channel, Receiver, Sender};

use themis::keygen::gen_ec_key_pair;
use themis::keys::EcdsaPublicKey;
use themis::secure_session::{
    SecureSession, SecureSessionState, SecureSessionTransport, TransportError,
};
use themis::ErrorKind;

#[test]
fn no_transport() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    // The client and the server.
    let mut client = SecureSession::with_transport(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::with_transport(name_server, &private_server, transport_server)
        .expect("Secure Session server");

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
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut client = SecureSession::with_transport(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::with_transport(name_server, &private_server, transport_server)
        .expect("Secure Session server");

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

#[test]
fn connection_state_reporting() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_server, &name_client, &public_client);
    expect_peer(&mut transport_client, &name_server, &public_server);

    let state_client = monitor_state_changes(&mut transport_client);
    let state_server = monitor_state_changes(&mut transport_server);

    let mut client = SecureSession::with_transport(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::with_transport(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.generate_connect_request().expect("connect request");
    assert_eq!(state_client.recv(), Ok(SecureSessionState::Negotiating));

    let connect_reply = server.negotiate(&connect_request).expect("connect reply");
    assert_eq!(state_server.recv(), Ok(SecureSessionState::Negotiating));

    let key_proposed = client.negotiate(&connect_reply).expect("key proposed");
    // No state change here, both parties are still negotiating...

    let key_accepted = server.negotiate(&key_proposed).expect("key accepted");
    assert_eq!(state_server.recv(), Ok(SecureSessionState::Established));

    let key_confirmed = client.negotiate(&key_accepted).expect("key confirmed");
    assert_eq!(state_client.recv(), Ok(SecureSessionState::Established));

    assert!(key_confirmed.is_empty());
}

#[test]
fn server_does_not_identify_client() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, _public_client) = gen_ec_key_pair().split();
    let (private_server, _public_server) = gen_ec_key_pair().split();

    let transport_client = MockTransport::new();

    let mut transport_server = MockTransport::new();
    expect_no_peers(&mut transport_server);

    let mut client = SecureSession::with_transport(&name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::with_transport(&name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.generate_connect_request().expect("connect request");

    let connect_error = server
        .negotiate(&connect_request)
        .expect_err("server error");

    assert_eq!(
        connect_error.kind(),
        ErrorKind::SessionGetPublicKeyForIdError
    );
}

#[test]
fn client_does_not_identify_server() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, _public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    expect_no_peers(&mut transport_client);

    let mut transport_server = MockTransport::new();
    expect_peer(&mut transport_server, &name_client, &public_client);

    let mut client = SecureSession::with_transport(&name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::with_transport(&name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.generate_connect_request().expect("connect request");
    let connect_reply = server.negotiate(&connect_request).expect("server reply");

    let negotiate_error = client.negotiate(&connect_reply).expect_err("client error");

    assert_eq!(
        negotiate_error.kind(),
        ErrorKind::SessionGetPublicKeyForIdError
    );
}

//
// MockTransport implementation
//

type GetPublicKeyForID = Box<dyn FnMut(&[u8]) -> Option<EcdsaPublicKey>>;
type SendData = Box<dyn FnMut(&[u8]) -> Result<usize, TransportError>>;
type ReceiveData = Box<dyn FnMut(&mut [u8]) -> Result<usize, TransportError>>;
type StateChanged = Box<dyn FnMut(SecureSessionState)>;

#[derive(Default)]
struct MockTransport {
    impl_get_public_key_for_id: Option<GetPublicKeyForID>,
    impl_send_data: Option<SendData>,
    impl_receive_data: Option<ReceiveData>,
    impl_state_changed: Option<StateChanged>,
}

impl SecureSessionTransport for MockTransport {
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
        if let Some(get_public_key_for_id) = &mut self.impl_get_public_key_for_id {
            get_public_key_for_id(id)
        } else {
            panic!("get_public_key_for_id() used but not implemented")
        }
    }

    fn send_data(&mut self, data: &[u8]) -> Result<usize, TransportError> {
        if let Some(send_data) = &mut self.impl_send_data {
            send_data(data)
        } else {
            panic!("send_data() used but not implemented")
        }
    }

    fn receive_data(&mut self, data: &mut [u8]) -> Result<usize, TransportError> {
        if let Some(receive_data) = &mut self.impl_receive_data {
            receive_data(data)
        } else {
            panic!("receive_data() used but not implemented")
        }
    }

    fn state_changed(&mut self, state: SecureSessionState) {
        if let Some(state_changed) = &mut self.impl_state_changed {
            state_changed(state)
        }
    }
}

impl MockTransport {
    fn new() -> Self {
        MockTransport::default()
    }

    fn when_get_public_key_for_id(
        &mut self,
        f: impl FnMut(&[u8]) -> Option<EcdsaPublicKey> + 'static,
    ) -> &mut Self {
        self.impl_get_public_key_for_id = Some(Box::new(f));
        self
    }

    fn when_send_data(
        &mut self,
        f: impl FnMut(&[u8]) -> Result<usize, TransportError> + 'static,
    ) -> &mut Self {
        self.impl_send_data = Some(Box::new(f));
        self
    }

    fn when_receive_data(
        &mut self,
        f: impl FnMut(&mut [u8]) -> Result<usize, TransportError> + 'static,
    ) -> &mut Self {
        self.impl_receive_data = Some(Box::new(f));
        self
    }

    fn when_state_changed(&mut self, f: impl FnMut(SecureSessionState) + 'static) -> &mut Self {
        self.impl_state_changed = Some(Box::new(f));
        self
    }
}

//
// MockTransport utilities
//

fn expect_peer(
    transport: &mut MockTransport,
    peer_id: impl AsRef<[u8]>,
    public_key: &EcdsaPublicKey,
) {
    let peer_id = peer_id.as_ref().to_vec();
    let public_key = public_key.clone();
    transport.when_get_public_key_for_id(move |id| {
        if peer_id == id {
            Some(public_key.clone())
        } else {
            None
        }
    });
}

fn expect_no_peers(transport: &mut MockTransport) {
    transport.when_get_public_key_for_id(|_| None);
}

fn connect_with_channels(client: &mut MockTransport, server: &mut MockTransport) {
    let (tx_client_server, rx_server_client) = channel();
    let (tx_server_client, rx_client_server) = channel();
    connect_channel(client, tx_client_server, rx_client_server);
    connect_channel(server, tx_server_client, rx_server_client);
}

fn connect_channel(transport: &mut MockTransport, tx: Sender<Vec<u8>>, rx: Receiver<Vec<u8>>) {
    transport.when_send_data(move |data| {
        tx.send(data.to_vec())?;
        Ok(data.len())
    });
    transport.when_receive_data(move |data| {
        let msg = rx.recv()?;
        if msg.len() > data.len() {
            return Err(TransportError::new("too small buffer"));
        }
        data[0..msg.len()].copy_from_slice(&msg);
        Ok(msg.len())
    });
}

fn monitor_state_changes(transport: &mut MockTransport) -> Receiver<SecureSessionState> {
    let (tx, rx) = channel();
    transport.when_state_changed(move |state| tx.send(state).expect("send error"));
    rx
}
