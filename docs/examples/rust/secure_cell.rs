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
use std::io::{self, Read, Write};

use themis::secure_cell::SecureCell;

fn main() {
    let matches = clap_app!(secure_cell =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Simple encryption with Secure Cell.")
        (@group mode =>
            (@arg encrypt: -e --encrypt "Encrypt the input [default]")
            (@arg decrypt: -d --decrypt "Decrypt the input")
        )
        (@arg password: -p --password <string> "Password to use")
        (@arg input:  +required "Input file")
        (@arg output: +required "Output file")
    ).get_matches();

    let encrypt = !matches.is_present("decrypt");
    let password = matches.value_of("password").unwrap();
    let input_path = matches.value_of("input").unwrap();
    let output_path = matches.value_of("output").unwrap();

    let cell = SecureCell::with_key(&password).seal();

    let input = read_file(&input_path).unwrap();
    let output = if encrypt {
        cell.encrypt(&input).unwrap()
    } else {
        cell.decrypt(&input).unwrap()
    };
    write_file(&output_path, &output).unwrap();

    if encrypt {
        eprintln!("encrypted {} as {}", input_path, output_path);
    } else {
        eprintln!("decrypted {} into {}", input_path, output_path);
    }
}

fn read_file(path: &str) -> Result<Vec<u8>, io::Error> {
    let mut file = File::open(path)?;
    let mut content = Vec::new();
    file.read_to_end(&mut content)?;
    Ok(content)
}

fn write_file(path: &str, data: &[u8]) -> Result<(), io::Error> {
    let mut file = File::create(path)?;
    file.write_all(data)?;
    Ok(())
}
