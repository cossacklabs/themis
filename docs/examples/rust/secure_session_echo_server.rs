// Copyright 2019 (c) rust-themis developers
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

use std::io::{Read, Write};
use std::net::{SocketAddr, TcpListener, TcpStream};

use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use clap::clap_app;
use lazy_static::lazy_static;
use log::{debug, error, info, warn};
use themis::keys::{EcdsaPrivateKey, EcdsaPublicKey};
use themis::secure_session::{
    SecureSession, SecureSessionState, SecureSessionTransport, TransportError,
};

const MAX_MESSAGE_SIZE: usize = 10000;
const TERMINATION_MESSAGE: &[u8] = b"please let me out of here\n";

// Peer identification data -- IDs and public keys -- should be obtained from trusted sources.
// The corresponding private key is used to identify this peer. It should be managed separately.
const CLIENT_ID: &[u8] = b"client";
const SERVER_ID: &[u8] = b"server";

lazy_static! {
    static ref SERVER_PRIVATE: EcdsaPrivateKey = EcdsaPrivateKey::try_from_slice(b"\x52\x45\x43\x32\x00\x00\x00\x2d\xd0\xfd\x93\xc6\x00\xae\x83\xb3\xef\xef\x06\x2c\x9d\x76\x63\xf2\x50\xd8\xac\x32\x6e\x73\x96\x60\x53\x77\x51\xe4\x34\x26\x7c\xf2\x9f\xb6\x96\xeb\xd8" as &[u8]).expect("client private key");
    static ref SERVER_PUBLIC: EcdsaPublicKey = EcdsaPublicKey::try_from_slice(b"\x55\x45\x43\x32\x00\x00\x00\x2d\xa5\xb3\x9b\x9d\x03\xcd\x34\xc5\xc1\x95\x6a\xb2\x50\x43\xf1\x4f\xe5\x88\x3a\x0f\xb1\x11\x8c\x35\x81\x82\xe6\x9e\x5c\x5a\x3e\x14\x06\xc5\xb3\x7d\xdd" as &[u8]).expect("server public key");
    static ref CLIENT_PUBLIC: EcdsaPublicKey = EcdsaPublicKey::try_from_slice(b"\x55\x45\x43\x32\x00\x00\x00\x2d\x10\xf4\x68\x8c\x02\x1c\xd0\x3b\x20\x84\xf2\x7a\x38\xbc\xf6\x39\x74\xbf\xc3\x13\xae\xb1\x00\x26\x78\x07\xe1\x7f\x63\xce\xe0\xb8\xac\x02\x10\x40\x10" as &[u8]).expect("client public key");
}

struct SocketTransport {
    socket: TcpStream,
}

impl SocketTransport {
    fn new(socket: TcpStream) -> Self {
        SocketTransport { socket }
    }
}

impl SecureSessionTransport for SocketTransport {
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
        match id {
            CLIENT_ID => Some(CLIENT_PUBLIC.clone()),
            SERVER_ID => Some(SERVER_PUBLIC.clone()),
            _ => None,
        }
    }

    // Note how transport callbacks do proper framing (by prefixing the data with its length) and
    // retrying (by using write_all() and read_exact() methods) to ensure that only entire Secure
    // Session messages are sent over the network.

    fn send_data(&mut self, data: &[u8]) -> Result<usize, TransportError> {
        if data.len() > u32::max_value() as usize {
            return Err(TransportError::new("buffer too big"));
        }
        self.socket.write_u32::<LittleEndian>(data.len() as u32)?;
        self.socket.write_all(data)?;
        debug!("{:?}: sent {} bytes", self.socket.peer_addr(), data.len());
        Ok(data.len())
    }

    fn receive_data(&mut self, data: &mut [u8]) -> Result<usize, TransportError> {
        let length = self.socket.read_u32::<LittleEndian>()? as usize;
        if data.len() < length {
            return Err(TransportError::new("insufficient buffer"));
        }
        self.socket.read_exact(&mut data[0..length])?;
        debug!("{:?}: received {} bytes", self.socket.peer_addr(), length);
        Ok(length)
    }

    fn state_changed(&mut self, state: SecureSessionState) {
        info!("{:?}: state changed: {:?}", self.socket.peer_addr(), state);
    }
}

fn main() {
    env_logger::init();

    let matches = clap_app!(secure_session_echo_server =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Echo server for Secure Session clients.")
        (@arg port: -p --port [number] "Listening port (default: 3432)")
    )
    .get_matches();

    let port = matches
        .value_of("port")
        .unwrap_or("3432")
        .parse()
        .expect("valid port");

    let listen_addr = SocketAddr::new([0; 16].into(), port);
    let listen_socket = TcpListener::bind(&listen_addr).expect("server listen");

    info!("listening on port {}", port);

    for client in listen_socket.incoming() {
        let client = match client {
            Ok(client) => client,
            Err(e) => {
                warn!("failed to accept client: {}", e);
                continue;
            }
        };
        let client_address = client.peer_addr();

        std::thread::spawn(move || {
            info!("{:?}: connected", client_address);

            let transport = SocketTransport::new(client);
            let mut session = SecureSession::new(&SERVER_ID, &SERVER_PRIVATE, transport)
                .expect("Secure Session server");

            while !session.is_established() {
                if let Err(e) = session.negotiate() {
                    error!("{:?}: negotiation failed: {}", client_address, e);
                    return;
                }
            }

            info!("{:?}: negotiated", client_address);

            loop {
                let message = match session.receive(MAX_MESSAGE_SIZE) {
                    Ok(m) => m,
                    Err(e) => {
                        error!("{:?}: failed to receive message: {}", client_address, e);
                        return;
                    }
                };
                debug!("{:?}: message: ({} bytes)", client_address, message.len());

                // Note that we use a special termination message to initiate orderly shutdown of
                // Secure Session. It is not secure to close the session unconditionally when the
                // socket appears to be disconnected.
                if message == TERMINATION_MESSAGE {
                    info!("{:?}: disconnected", client_address);
                    break;
                }

                if let Err(e) = session.send(&message) {
                    error!("{:?}: failed to send message: {}", client_address, e);
                    return;
                }
            }
        });
    }
}
