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

use std::process::exit;

use clap::clap_app;
use themis::secure_cell::SecureCell;

fn main() {
    let matches = clap_app!(scell_context_string_echo =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Secure Cell echo testing tool (context imprint mode).")
        (@arg mode:    +required "<enc|dec>")
        (@arg key:     +required "master key")
        (@arg message: +required "message to encrypt or decrypt")
        (@arg context: +required "user context")
    )
    .get_matches();

    let mode = matches.value_of("mode").unwrap();
    let key = matches.value_of("key").unwrap();
    let message = matches.value_of("message").unwrap();
    let context = matches.value_of("context").unwrap();

    let cell = SecureCell::with_key(&key)
        .unwrap_or_else(|_| {
            eprintln!("invalid parameters: empty master key");
            exit(1);
        })
        .context_imprint();

    match mode {
        "enc" => {
            let encrypted = cell
                .encrypt_with_context(&message, &context)
                .unwrap_or_else(|error| {
                    eprintln!("failed to encrypt message: {}", error);
                    exit(1);
                });
            println!("{}", base64::encode(&encrypted));
        }
        "dec" => {
            let decoded_message = base64::decode(&message).unwrap_or_else(|error| {
                eprintln!("failed to decode message: {}", error);
                exit(1);
            });
            let decrypted = cell
                .decrypt_with_context(&decoded_message, &context)
                .unwrap_or_else(|error| {
                    eprintln!("failed to decrypt message: {}", error);
                    exit(1);
                });
            println!("{}", std::str::from_utf8(&decrypted).expect("UTF-8 string"));
        }
        other => {
            eprintln!("invalid mode {}, use \"enc\" or \"dec\"", other);
            exit(1);
        }
    }
}
