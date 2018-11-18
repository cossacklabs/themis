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
use std::thread;

use themis::keys::{PublicKey, SecretKey};
use themis::secure_message::{SecureSign, SecureVerify};

fn main() {
    env_logger::init();

    let matches = clap_app!(secure_message_client_verify =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Secure Message chat client (sign/verify).")
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

    let socket = UdpSocket::bind("localhost:0").expect("client socket");
    socket.connect(&remote_addr).expect("client connection");

    let receive_socket = socket;
    let relay_socket = receive_socket.try_clone().unwrap();

    // Each of the threads is using its own object for message processing.
    // Also note that SecureSign/SecureVerify API is deliberately different from SecureMessage.
    let receive_secure = SecureVerify::new(public_key);
    let relay_secure = SecureSign::new(secret_key);

    let receive = thread::spawn(move || {
        let receive_message = || -> io::Result<()> {
            let buffer = recv(&receive_socket)?;
            let message = receive_secure.verify(&buffer).map_err(themis_as_io_error)?;
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
            let message = relay_secure.sign(&buffer).map_err(themis_as_io_error)?;
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
