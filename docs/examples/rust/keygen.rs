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
extern crate themis;

use std::fs::File;
use std::io::{self, Write};

use themis::keygen::gen_ec_key_pair;

fn main() {
    let matches = clap_app!(keygen =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Generating ECDSA key pairs.")
        (@arg secret: --secret [path] "Secret key file (default: secret.key)")
        (@arg public: --public [path] "Public key file (default: public.key)")
    ).get_matches();
    let secret_path = matches.value_of("secret").unwrap_or("secret.key");
    let public_path = matches.value_of("public").unwrap_or("public.key");

    let (secret_key, public_key) = gen_ec_key_pair().split();

    match write_file(&secret_key, &secret_path) {
        Ok(_) => eprintln!("wrote secret key to {}", secret_path),
        Err(e) => eprintln!("failed to write secret key to {}: {}", secret_path, e),
    }
    match write_file(&public_key, &public_path) {
        Ok(_) => eprintln!("wrote public key to {}", public_path),
        Err(e) => eprintln!("failed to write public key to {}: {}", public_path, e),
    }
}

fn write_file<K: AsRef<[u8]>>(key: K, path: &str) -> io::Result<()> {
    let mut file = File::create(path)?;
    file.write_all(key.as_ref())?;
    Ok(())
}
