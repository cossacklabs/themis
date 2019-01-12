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

#[macro_use]
extern crate clap;
extern crate env_logger;
#[macro_use]
extern crate log;

use std::collections::HashSet;
use std::io;
use std::net::{SocketAddr, UdpSocket};

fn main() {
    env_logger::init();

    let matches = clap_app!(secure_message_server =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Relay server for Secure Message chat client.")
        (@arg port: -p --port [number] "Listening port (default: 7573)")
    ).get_matches();

    let port = matches.value_of("port").unwrap_or("7573").parse().unwrap();
    let listen_addr = SocketAddr::new([0; 16].into(), port);

    let socket = UdpSocket::bind(&listen_addr).expect("server listen");
    let mut peers = HashSet::new();
    let mut process_message = || -> io::Result<()> {
        let (message, sender) = recv_from(&socket)?;

        // We never actually remove peers from this list. It does not do any visible harm because
        // UDP allows sending datagrams to any address. However, this does add up to network spam.
        // A proper chat would track peer connections in some way, but we're too lazy for that.
        if peers.insert(sender) {
            info!("new peer: {}", sender);
        }

        for peer in &peers {
            // Avoid relaying the message to the original sender.
            if *peer == sender {
                continue;
            }
            socket.send_to(&message, peer)?;
        }

        Ok(())
    };

    info!("listening on port {}", port);
    loop {
        if let Err(e) = process_message() {
            error!("failed to process message: {}", e);
            break;
        }
    }
}

fn recv_from(socket: &UdpSocket) -> io::Result<(Vec<u8>, SocketAddr)> {
    let mut message = vec![0; 65536];
    let (length, sender) = socket.recv_from(&mut message)?;
    message.truncate(length);
    Ok((message, sender))
}
