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
extern crate themis;
#[macro_use]
extern crate log;

use std::fs::File;
use std::io::{self, Read, Write};
use std::net::UdpSocket;
use std::sync::Arc;
use std::thread;

use themis::keys::{KeyPair, PublicKey, SecretKey};
use themis::secure_message::SecureMessage;

fn main() {
    env_logger::init();

    let matches = clap_app!(secure_message_client_encrypt =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Secure Message chat client (encrypt).")
        (@arg secret: --secret [path] "Secret key file (default: secret.key)")
        (@arg public: --public [path] "Public key file (default: public.key)")
        (@arg address: -c --connect [addr] "Relay server address (default: localhost:7573)")
    ).get_matches();

    let secret_path = matches.value_of("secret").unwrap_or("secret.key");
    let public_path = matches.value_of("public").unwrap_or("public.key");
    let remote_addr = matches.value_of("address").unwrap_or("localhost:7573");

    let secret_key = read_file(&secret_path).expect("read secret key");
    let secret_key = SecretKey::try_from_slice(secret_key).expect("parse secret key");
    let public_key = read_file(&public_path).expect("read public key");
    let public_key = PublicKey::try_from_slice(public_key).expect("parse public key");
    let key_pair = KeyPair::try_join(secret_key, public_key).expect("matching keys");

    let socket = UdpSocket::bind("localhost:0").expect("client socket");
    socket.connect(&remote_addr).expect("client connection");

    let receive_socket = socket;
    let relay_socket = receive_socket.try_clone().unwrap();

    // SecureMessage objects are stateless so they can be shared between threads without issues.
    // Also note that SecureMessage API is deliberately different from SecureSign/SecureVerify.
    let receive_secure = Arc::new(SecureMessage::new(key_pair));
    let relay_secure = receive_secure.clone();

    let receive = thread::spawn(move || {
        let receive_message = || -> io::Result<()> {
            let buffer = recv(&receive_socket)?;
            let message = receive_secure.unwrap(&buffer).map_err(themis_as_io_error)?;
            io::stdout().write_all(&message)?;
            Ok(())
        };
        loop {
            if let Err(e) = receive_message() {
                error!("failed to receive message: {}", e);
                break;
            }
        }
    });

    let relay = thread::spawn(move || {
        let relay_message = || -> io::Result<()> {
            let mut buffer = String::new();
            io::stdin().read_line(&mut buffer)?;
            let message = relay_secure.wrap(&buffer).map_err(themis_as_io_error)?;
            relay_socket.send(&message)?;
            Ok(())
        };
        loop {
            if let Err(e) = relay_message() {
                error!("failed to relay message: {}", e);
                break;
            }
        }
    });

    receive.join().unwrap();
    relay.join().unwrap();
}

fn read_file(path: &str) -> io::Result<Vec<u8>> {
    let mut file = File::open(path)?;
    let mut content = Vec::new();
    file.read_to_end(&mut content)?;
    Ok(content)
}

fn recv(socket: &UdpSocket) -> io::Result<Vec<u8>> {
    let mut message = vec![0; 65536];
    let length = socket.recv(&mut message)?;
    message.truncate(length);
    Ok(message)
}

fn themis_as_io_error(e: themis::Error) -> io::Error {
    io::Error::new(io::ErrorKind::Other, e)
}
