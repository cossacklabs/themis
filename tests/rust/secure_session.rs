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

#![allow(clippy::needless_borrow)]

use std::sync::mpsc::{channel, Receiver, Sender};

use themis::keygen::gen_ec_key_pair;
use themis::keys::EcdsaPublicKey;
use themis::secure_session::{
    SecureSession, SecureSessionState, SecureSessionTransport, TransportError,
};
use themis::ErrorKind;

#[test]
fn invalid_client_id() {
    let (private, _) = gen_ec_key_pair().split();
    let transport = MockTransport::new();

    let error = SecureSession::new(&[], &private, transport)
        .expect_err("construction with empty client ID");

    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

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
    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    assert!(!client.is_established());
    assert!(!server.is_established());
    assert_eq!(client.remote_peer_id(), Ok(None));
    assert_eq!(server.remote_peer_id(), Ok(None));

    // Connection and key negotiation sequence.
    let connect_request = client.connect_request().expect("connect request");
    let connect_reply = server
        .negotiate_reply(&connect_request)
        .expect("connect reply");
    let key_proposed = client
        .negotiate_reply(&connect_reply)
        .expect("key proposed");
    let key_accepted = server.negotiate_reply(&key_proposed).expect("key accepted");
    let key_confirmed = client
        .negotiate_reply(&key_accepted)
        .expect("key confirmed");
    assert!(key_confirmed.is_empty());

    assert!(client.is_established());
    assert!(server.is_established());
    assert_eq!(
        client.remote_peer_id(),
        Ok(Some(name_server.as_bytes().to_vec()))
    );
    assert_eq!(
        server.remote_peer_id(),
        Ok(Some(name_client.as_bytes().to_vec()))
    );

    // Try sending a message back and forth.
    let plaintext = b"test message please ignore";

    let wrapped = client.wrap(&plaintext).expect("wrap 1 -> 2 message");
    let unwrapped = server.unwrap(&wrapped).expect("unwrap 1 -> 2 message");
    assert_eq!(unwrapped, plaintext);

    let wrapped = server.wrap(&plaintext).expect("wrap 2 -> 1 message");
    let unwrapped = client.unwrap(&wrapped).expect("unwrap 2 -> 1 message");
    assert_eq!(unwrapped, plaintext);

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

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    assert!(!client.is_established());
    assert!(!server.is_established());

    // Establishing connection.
    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

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

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.connect_request().expect("connect request");
    assert_eq!(state_client.recv(), Ok(SecureSessionState::Negotiating));

    let connect_reply = server
        .negotiate_reply(&connect_request)
        .expect("connect reply");
    assert_eq!(state_server.recv(), Ok(SecureSessionState::Negotiating));

    let key_proposed = client
        .negotiate_reply(&connect_reply)
        .expect("key proposed");
    // No state change here, both parties are still negotiating...

    let key_accepted = server.negotiate_reply(&key_proposed).expect("key accepted");
    assert_eq!(state_server.recv(), Ok(SecureSessionState::Established));

    let key_confirmed = client
        .negotiate_reply(&key_accepted)
        .expect("key confirmed");
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

    let mut client = SecureSession::new(&name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(&name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.connect_request().expect("connect request");

    let connect_error = server
        .negotiate_reply(&connect_request)
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

    let mut client = SecureSession::new(&name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(&name_server, &private_server, transport_server)
        .expect("Secure Session server");

    let connect_request = client.connect_request().expect("connect request");
    let connect_reply = server
        .negotiate_reply(&connect_request)
        .expect("server reply");

    let negotiate_error = client
        .negotiate_reply(&connect_reply)
        .expect_err("client error");

    assert_eq!(
        negotiate_error.kind(),
        ErrorKind::SessionGetPublicKeyForIdError
    );
}

#[test]
fn forward_error_send_at_connection() {
    let name_client = "client";
    let (private_client, _public_client) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();

    let mut next_client_send = override_send(&mut transport_client);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");

    next_client_send.will_be(|_| Err(TransportError::new("error")));

    let error = client.connect().expect_err("client-side connection");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn forward_error_receive_at_connection() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_server_receive = override_receive(&mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    // Establishing connection.
    client.connect().expect("client-side connection");

    next_server_receive.will_be(|_| Err(TransportError::new("error")));

    let error = server
        .negotiate()
        .expect_err("failed to negotiate transport");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn forward_error_send_at_negotiation() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_server_send = override_send(&mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");

    next_server_send.will_be(|_| Err(TransportError::new("error")));

    let error = server
        .negotiate()
        .expect_err("failed to negotiate transport");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn forward_error_receive_at_negotiation() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_client_receive = override_receive(&mut transport_client);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");

    next_client_receive.will_be(|_| Err(TransportError::new("error")));

    let error = client
        .negotiate()
        .expect_err("failed to negotiate transport");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn forward_error_send_at_exchange() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_client_send = override_send(&mut transport_client);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

    assert!(client.is_established());
    assert!(server.is_established());

    next_client_send.will_be(|_| Err(TransportError::new("error")));

    let error = client
        .send(b"test message")
        .expect_err("failed to send message");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn forward_error_receive_at_exchange() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_server_receive = override_receive(&mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

    assert!(client.is_established());
    assert!(server.is_established());

    client
        .send(b"test message please ignore")
        .expect("client send");

    next_server_receive.will_be(|_| Err(TransportError::new("error")));

    let error = server.receive(1024).expect_err("failed to receive message");

    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::new("error"))
    );
}

#[test]
fn cannot_send_empty_message() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

    assert!(client.is_established());
    assert!(server.is_established());

    let error = client.send(b"").expect_err("failed to send message");

    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

#[test]
fn cannot_receive_empty_message() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut next_client_receive = override_receive(&mut transport_client);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

    assert!(client.is_established());
    assert!(server.is_established());

    next_client_receive.will_be(|_| Ok(0));

    let error = client.receive(1024).expect_err("failed to receive message");

    assert_eq!(error.kind(), ErrorKind::InvalidParameter);
}

#[test]
fn panic_in_get_pubkey_by_id_client() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, _public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    transport_client.when_get_public_key_for_id(|_| panic!());
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");

    let error = client.negotiate().expect_err("catch client panic");
    assert_eq!(error.kind(), ErrorKind::SessionGetPublicKeyForIdError);
}

#[test]
fn panic_in_get_pubkey_by_id_server() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, _public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    transport_server.when_get_public_key_for_id(|_| panic!());

    connect_with_channels(&mut transport_client, &mut transport_server);

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client connect");

    let error = server.negotiate().expect_err("catch server panic");
    assert_eq!(error.kind(), ErrorKind::SessionGetPublicKeyForIdError);
}

#[test]
fn panic_in_send_data() {
    let (private_client, _) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();

    transport_client.when_send_data(|_| panic!());

    let mut client = SecureSession::new("client", &private_client, transport_client)
        .expect("Secure Session client");

    let error = client.connect().expect_err("catch client panic");
    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::unspecified())
    );
}

#[test]
fn panic_in_receive_data() {
    let (private_server, _) = gen_ec_key_pair().split();

    let mut transport_server = MockTransport::new();

    transport_server.when_receive_data(|_| panic!());

    let mut server = SecureSession::new("server", &private_server, transport_server)
        .expect("Secure Session server");

    let error = server.negotiate().expect_err("catch server panic");
    assert_eq!(
        error.kind(),
        ErrorKind::SessionTransportError(TransportError::unspecified())
    );
}

#[test]
fn panic_in_status_change() {
    let (name_client, name_server) = ("client", "server");
    let (private_client, public_client) = gen_ec_key_pair().split();
    let (private_server, public_server) = gen_ec_key_pair().split();

    let mut transport_client = MockTransport::new();
    let mut transport_server = MockTransport::new();

    expect_peer(&mut transport_client, &name_server, &public_server);
    expect_peer(&mut transport_server, &name_client, &public_client);

    connect_with_channels(&mut transport_client, &mut transport_server);

    transport_client.when_state_changed(|_| panic!());
    transport_server.when_state_changed(|_| panic!());

    let mut client = SecureSession::new(name_client, &private_client, transport_client)
        .expect("Secure Session client");
    let mut server = SecureSession::new(name_server, &private_server, transport_server)
        .expect("Secure Session server");

    client.connect().expect("client-side connection");
    server.negotiate().expect("connect reply");
    client.negotiate().expect("key proposed");
    server.negotiate().expect("key accepted");
    client.negotiate().expect("key confirmed");

    // Any panics in state_change callback should be ignored.
    assert!(client.is_established());
    assert!(server.is_established());
}

//
// MockTransport implementation
//

type GetPublicKeyForId = Box<dyn FnMut(&[u8]) -> Option<EcdsaPublicKey> + Send>;
type SendData = Box<dyn FnMut(&[u8]) -> Result<usize, TransportError> + Send>;
type ReceiveData = Box<dyn FnMut(&mut [u8]) -> Result<usize, TransportError> + Send>;
type StateChanged = Box<dyn FnMut(SecureSessionState) + Send>;

#[derive(Default)]
struct MockTransport {
    impl_get_public_key_for_id: Option<GetPublicKeyForId>,
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
        f: impl FnMut(&[u8]) -> Option<EcdsaPublicKey> + Send + 'static,
    ) -> &mut Self {
        self.impl_get_public_key_for_id = Some(Box::new(f));
        self
    }

    fn when_send_data(
        &mut self,
        f: impl FnMut(&[u8]) -> Result<usize, TransportError> + Send + 'static,
    ) -> &mut Self {
        self.impl_send_data = Some(Box::new(f));
        self
    }

    fn when_receive_data(
        &mut self,
        f: impl FnMut(&mut [u8]) -> Result<usize, TransportError> + Send + 'static,
    ) -> &mut Self {
        self.impl_receive_data = Some(Box::new(f));
        self
    }

    fn when_state_changed(
        &mut self,
        f: impl FnMut(SecureSessionState) + Send + 'static,
    ) -> &mut Self {
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

struct SendDataOverride(Sender<SendData>);

impl SendDataOverride {
    fn will_be(&mut self, f: impl FnMut(&[u8]) -> Result<usize, TransportError> + Send + 'static) {
        self.0.send(Box::new(f)).expect("unexpected send error");
    }
}

fn override_send(transport: &mut MockTransport) -> SendDataOverride {
    let (tx, rx) = channel::<SendData>();
    let mut old_send = transport.impl_send_data.take();
    transport.when_send_data(move |data| {
        if let Ok(mut override_send) = rx.try_recv() {
            override_send(data)
        } else if let Some(ref mut fallback_send) = old_send {
            fallback_send(data)
        } else {
            panic!("no send_data() fallback");
        }
    });
    SendDataOverride(tx)
}

struct ReceiveDataOverride(Sender<ReceiveData>);

impl ReceiveDataOverride {
    fn will_be(
        &mut self,
        f: impl FnMut(&mut [u8]) -> Result<usize, TransportError> + Send + 'static,
    ) {
        self.0.send(Box::new(f)).expect("unexpected send error");
    }
}

fn override_receive(transport: &mut MockTransport) -> ReceiveDataOverride {
    let (tx, rx) = channel::<ReceiveData>();
    let mut old_receive = transport.impl_receive_data.take();
    transport.when_receive_data(move |data| {
        if let Ok(mut override_receive) = rx.try_recv() {
            override_receive(data)
        } else if let Some(ref mut fallback_receive) = old_receive {
            fallback_receive(data)
        } else {
            panic!("no receive_data() fallback");
        }
    });
    ReceiveDataOverride(tx)
}

fn monitor_state_changes(transport: &mut MockTransport) -> Receiver<SecureSessionState> {
    let (tx, rx) = channel();
    transport.when_state_changed(move |state| tx.send(state).expect("send error"));
    rx
}
