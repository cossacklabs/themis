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

use std::io::{self, Read, Write};
use std::net::TcpStream;

use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use clap::clap_app;
use lazy_static::lazy_static;
use log::{debug, info};
use themis::keys::{EcdsaPrivateKey, EcdsaPublicKey};
use themis::secure_session::{SecureSession, SecureSessionTransport};

const MAX_MESSAGE_SIZE: usize = 10000;
const TERMINATION_MESSAGE: &[u8] = b"please let me out of here\n";

// Peer identification data -- IDs and public keys -- should be obtained from trusted sources.
// The corresponding private key is used to identify this peer. It should be managed separately.
const CLIENT_ID: &[u8] = b"client";
const SERVER_ID: &[u8] = b"server";

lazy_static! {
    static ref CLIENT_PRIVATE: EcdsaPrivateKey = EcdsaPrivateKey::try_from_slice(b"\x52\x45\x43\x32\x00\x00\x00\x2d\x00\xb2\x7f\x81\x00\x60\x9d\xe7\x7a\x39\x93\x68\xfc\x25\xd1\x79\x88\x6d\xfb\xf6\x19\x35\x53\x74\x10\xfc\x5b\x44\xe1\xf6\xf4\x4e\x59\x8d\x94\x99\x4f" as &[u8]).expect("client private key");
    static ref CLIENT_PUBLIC: EcdsaPublicKey = EcdsaPublicKey::try_from_slice(b"\x55\x45\x43\x32\x00\x00\x00\x2d\x10\xf4\x68\x8c\x02\x1c\xd0\x3b\x20\x84\xf2\x7a\x38\xbc\xf6\x39\x74\xbf\xc3\x13\xae\xb1\x00\x26\x78\x07\xe1\x7f\x63\xce\xe0\xb8\xac\x02\x10\x40\x10" as &[u8]).expect("client public key");
    static ref SERVER_PUBLIC: EcdsaPublicKey = EcdsaPublicKey::try_from_slice(b"\x55\x45\x43\x32\x00\x00\x00\x2d\xa5\xb3\x9b\x9d\x03\xcd\x34\xc5\xc1\x95\x6a\xb2\x50\x43\xf1\x4f\xe5\x88\x3a\x0f\xb1\x11\x8c\x35\x81\x82\xe6\x9e\x5c\x5a\x3e\x14\x06\xc5\xb3\x7d\xdd" as &[u8]).expect("server public key");
}

struct ExpectServer;

impl SecureSessionTransport for ExpectServer {
    fn get_public_key_for_id(&mut self, id: &[u8]) -> Option<EcdsaPublicKey> {
        if id == SERVER_ID {
            Some(SERVER_PUBLIC.clone())
        } else {
            None
        }
    }
}

fn main() {
    env_logger::init();

    let matches = clap_app!(secure_session_echo_client =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Echo client for Secure Session server.")
        (@arg address: -c --connect [addr] "Echo server address (default: localhost:3432)")
    )
    .get_matches();

    let remote_addr = matches.value_of("address").unwrap_or("localhost:3432");

    info!("connecting to {:?}", remote_addr);

    let mut socket = TcpStream::connect(&remote_addr).expect("client connection");

    let mut session = SecureSession::new(&CLIENT_ID, &CLIENT_PRIVATE, ExpectServer)
        .expect("Secure Session client");
    let mut buffer = [0; MAX_MESSAGE_SIZE];

    let request = session.connect_request().expect("connect request");
    write_framed(&mut socket, &request).expect("send request");

    loop {
        let reply = read_framed(&mut socket, &mut buffer).expect("receive reply");
        let response = session.negotiate_reply(&reply).expect("negotiate");
        if session.is_established() {
            break;
        }
        write_framed(&mut socket, &response).expect("send response");
    }

    info!("negotiation complete");

    loop {
        let mut line = String::new();
        io::stdin().read_line(&mut line).expect("read from stdin");
        if line.is_empty() {
            break;
        }

        let message = session.wrap(line.as_bytes()).expect("wrap outgoing");
        write_framed(&mut socket, &message).expect("write to socket");

        let reply = read_framed(&mut socket, &mut buffer).expect("read from socket");
        let reply = session.unwrap(&reply).expect("unwrap incoming");

        io::stdout().write_all(&reply).expect("write to stdout");
    }

    let quit = session.wrap(TERMINATION_MESSAGE).expect("wrap termination");
    write_framed(&mut socket, &quit).expect("send termination");
}

// Note how client has to implement the framing scheme used by the server. It is necessary to send
// and receive complete Secure Session messages over the network.

fn write_framed(socket: &mut TcpStream, data: &[u8]) -> io::Result<()> {
    if data.len() > u32::max_value() as usize {
        return Err(io::Error::new(io::ErrorKind::Other, "buffer too big"));
    }
    socket.write_u32::<LittleEndian>(data.len() as u32)?;
    socket.write_all(data)?;
    debug!("{:?}: sent {} bytes", socket.peer_addr(), data.len());
    Ok(())
}

fn read_framed<'a>(socket: &mut TcpStream, data: &'a mut [u8]) -> io::Result<&'a [u8]> {
    let length = socket.read_u32::<LittleEndian>()? as usize;
    if data.len() < length {
        return Err(io::Error::new(io::ErrorKind::Other, "insufficient buffer"));
    }
    socket.read_exact(&mut data[0..length])?;
    debug!("{:?}: received {} bytes", socket.peer_addr(), length);
    Ok(&data[0..length])
}
