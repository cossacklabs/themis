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

use std::fs::File;
use std::io::{self, Read};
use std::process::exit;

use clap::clap_app;
use themis::keys::{KeyPair, PrivateKey, PublicKey};
use themis::secure_message::{SecureMessage, SecureSign, SecureVerify};

fn main() {
    let matches = clap_app!(smessage_encryption =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Secure Message multitool.")
        (@arg command:    +required "Action: enc | dec | sign | verify")
        (@arg private_key: +required "Path to file with recipient's private key")
        (@arg public_key: +required "Path to file with sender's public key")
        (@arg message:    +required "The message to process")
    )
    .get_matches();

    let command = matches.value_of("command").unwrap();
    let private_key_path = matches.value_of("private_key").unwrap();
    let public_key_path = matches.value_of("public_key").unwrap();
    let message = matches.value_of("message").unwrap();

    let private_key = read_file(private_key_path).expect("read private key");
    let private_key = PrivateKey::try_from_slice(private_key).expect("parse private key");
    let public_key = read_file(public_key_path).expect("read public key");
    let public_key = PublicKey::try_from_slice(public_key).expect("parse public key");

    match command {
        "enc" => {
            let key_pair = KeyPair::try_join(private_key, public_key).expect("matching keys");
            let encrypter = SecureMessage::new(key_pair);

            let encrypted = encrypter.encrypt(&message).unwrap_or_else(|error| {
                eprintln!("failed to encrypt message: {error}");
                exit(1);
            });

            println!("{}", base64::encode(&encrypted));
        }
        "dec" => {
            let key_pair = KeyPair::try_join(private_key, public_key).expect("matching keys");
            let encrypter = SecureMessage::new(key_pair);

            let decoded_message = base64::decode(&message).unwrap_or_else(|error| {
                eprintln!("failed to decode message: {error}");
                exit(1);
            });
            let decrypted = encrypter.decrypt(&decoded_message).unwrap_or_else(|error| {
                eprintln!("failed to decrypt message: {error}");
                exit(1);
            });

            println!("{}", std::str::from_utf8(&decrypted).expect("UTF-8 string"));
        }
        "sign" => {
            let signer = SecureSign::new(private_key);

            let signed = signer.sign(&message).unwrap_or_else(|error| {
                eprintln!("failed to sign message: {error}");
                exit(1);
            });

            println!("{}", base64::encode(&signed));
        }
        "verify" => {
            let signer = SecureVerify::new(public_key);

            let decoded_message = base64::decode(&message).unwrap_or_else(|error| {
                eprintln!("failed to decode message: {error}");
                exit(1);
            });
            let verified = signer.verify(&decoded_message).unwrap_or_else(|error| {
                eprintln!("failed to verify message: {error}");
                exit(1);
            });

            println!("{}", std::str::from_utf8(&verified).expect("UTF-8 string"));
        }
        other => {
            eprintln!("invalid command {other}, use \"enc\", \"dec\", \"sign\", \"verify\"",);
            exit(1);
        }
    }
}

fn read_file(path: &str) -> Result<Vec<u8>, io::Error> {
    let mut file = File::open(path)?;
    let mut content = Vec::new();
    file.read_to_end(&mut content)?;
    Ok(content)
}
