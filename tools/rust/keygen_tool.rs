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

use std::fs::OpenOptions;
use std::io::{self, Write};
#[cfg(unix)]
use std::os::unix::fs::OpenOptionsExt;

use clap::clap_app;
use themis::keygen::gen_ec_key_pair;

fn main() {
    let matches = clap_app!(keygen_tool =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Generating ECDSA key pairs.")
        (@arg private: "Private key file (default: key)")
        (@arg public: "Public key file (default: key.pub)")
    )
    .get_matches();
    let private_path = matches.value_of("private").unwrap_or("key");
    let default_path = private_path.to_owned() + ".pub";
    let public_path = matches.value_of("public").unwrap_or(&default_path);

    let (private_key, public_key) = gen_ec_key_pair().split();

    match write_file(&private_key, private_path, 0o400) {
        Ok(_) => {}
        Err(e) => eprintln!("failed to write private key to {private_path}: {e}"),
    }
    match write_file(&public_key, public_path, 0o666) {
        Ok(_) => {}
        Err(e) => eprintln!("failed to write public key to {public_path}: {e}"),
    }
}

fn write_file(key: impl AsRef<[u8]>, path: &str, mode: u32) -> io::Result<()> {
    let mut options = OpenOptions::new();
    options.create(true);
    options.truncate(true);
    options.write(true);
    #[cfg(unix)]
    options.mode(mode);

    let mut file = options.open(path)?;
    file.write_all(key.as_ref())?;
    Ok(())
}
