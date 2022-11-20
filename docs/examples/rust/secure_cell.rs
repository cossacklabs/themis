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

#![allow(clippy::needless_borrow)]

use themis::keys::SymmetricKey;
use themis::secure_cell::SecureCell;

fn main() -> themis::Result<()> {
    let message = b"We must all play our assigned roles. Are you a pawn or a queen?".as_ref();
    let context = b"Consider the prospect that you have been misled, Alice.".as_ref();
    let key = SymmetricKey::new();
    let passphrase = "Then ask, by whom?";

    println!("# Secure Cell in Seal mode");
    println!();

    println!("## Master key API");
    println!();
    {
        let scell_mk = SecureCell::with_key(&key)?.seal();

        println!("Encoded:   {}", base64::encode(&message));

        let encrypted_message = scell_mk.encrypt(&message)?;
        println!("Encrypted: {}", base64::encode(&encrypted_message));

        let decrypted_message = scell_mk.decrypt(&encrypted_message)?;
        println!("Decrypted: {}", as_str(&decrypted_message));
        assert_eq!(decrypted_message, message);
    }
    println!();

    println!("## Passphrase API");
    {
        let scell_pw = SecureCell::with_passphrase(&passphrase)?.seal();

        println!("Encoded:   {}", base64::encode(&message));

        let encrypted_message = scell_pw.encrypt(&message)?;
        println!("Encrypted: {}", base64::encode(&encrypted_message));

        let decrypted_message = scell_pw.decrypt(&encrypted_message)?;
        println!("Decrypted: {}", as_str(&decrypted_message));
        assert_eq!(decrypted_message, message);
    }
    println!();

    println!("# Secure Cell in Token Protect mode");
    println!();
    {
        let scell_tp = SecureCell::with_key(&key)?.token_protect();

        println!("Encoded:    {}", base64::encode(&message));

        let (encrypted_message, auth_token) = scell_tp.encrypt(&message)?;
        println!("Encrypted:  {}", base64::encode(&encrypted_message));
        println!("Auth token: {}", base64::encode(&auth_token));

        let decrypted_message = scell_tp.decrypt(encrypted_message, auth_token)?;
        println!("Decrypted:  {}", as_str(&decrypted_message));
        assert_eq!(decrypted_message, message);
    }
    println!();

    println!("# Secure Cell in Context Imprint mode");
    println!();
    {
        let scell_ci = SecureCell::with_key(&key)?.context_imprint();

        println!("Encoded:   {}", base64::encode(&message));

        let encrypted_message = scell_ci.encrypt_with_context(&message, &context)?;
        println!("Encrypted: {}", base64::encode(&encrypted_message));

        let decrypted_message = scell_ci.decrypt_with_context(&encrypted_message, &context)?;
        println!("Decrypted: {}", as_str(&decrypted_message));
        assert_eq!(decrypted_message, message);
    }
    println!();

    Ok(())
}

fn as_str(utf8_bytes: &[u8]) -> &str {
    std::str::from_utf8(utf8_bytes).expect("valid UTF-8")
}
