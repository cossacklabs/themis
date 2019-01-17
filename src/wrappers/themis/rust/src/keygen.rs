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

//! Generating key material.
//!
//! Themis supports two kinds of asymmetric cryptography keys: Elliptic Curve (ECDSA) and RSA.
//! These keys are used by [`SecureMessage`] and [`SecureSession`] objects.
//!
//! This module contains functions for securely generating random key pairs. Note that managing
//! resulting keys is _your_ responsibility. You have to make sure that secret keys are kept
//! secret when distributed to your users, and that public keys that you use come from trusted
//! sources. You can consult [our guidelines][key-management] for some advice on key management.
//!
//! [`SecureMessage`]: ../secure_message/index.html
//! [`SecureSession`]: ../secure_session/index.html
//! [key-management]: https://github.com/cossacklabs/themis/wiki/Key-management
//!
//! # Examples
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! use themis::keygen::gen_ec_key_pair;
//! use themis::secure_message::SecureMessage;
//!
//! // Here we generate a new random Elliptic Curve key pair.
//! let key_pair = gen_ec_key_pair();
//!
//! let secure = SecureMessage::new(key_pair);
//!
//! let encrypted = secure.wrap(b"message")?;
//! let decrypted = secure.unwrap(&encrypted)?;
//! assert_eq!(decrypted, b"message");
//! # Ok(())
//! # }
//! ```

use std::ptr;

use bindings::{themis_gen_ec_key_pair, themis_gen_rsa_key_pair};

use crate::error::{Error, ErrorKind, Result};
use crate::keys::{
    EcdsaKeyPair, EcdsaPublicKey, EcdsaSecretKey, RsaKeyPair, RsaPublicKey, RsaSecretKey,
};

/// Generates a pair of RSA keys.
///
/// RSA keys are supported only by Secure Message objects.
///
/// # Panics
///
/// This function may panic in case of unrecoverable errors inside the library (e.g., out-of-memory
/// or assertion violations).
pub fn gen_rsa_key_pair() -> RsaKeyPair {
    match try_gen_rsa_key_pair() {
        Ok(keys) => keys,
        Err(e) => panic!("themis_gen_rsa_key_pair() failed: {}", e),
    }
}

/// Generates a secret-public pair of RSA keys.
fn try_gen_rsa_key_pair() -> Result<RsaKeyPair> {
    let mut secret_key = Vec::new();
    let mut public_key = Vec::new();
    let mut secret_key_len = 0;
    let mut public_key_len = 0;

    unsafe {
        let status = themis_gen_rsa_key_pair(
            ptr::null_mut(),
            &mut secret_key_len,
            ptr::null_mut(),
            &mut public_key_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    secret_key.reserve(secret_key_len);
    public_key.reserve(secret_key_len);

    unsafe {
        let status = themis_gen_rsa_key_pair(
            secret_key.as_mut_ptr(),
            &mut secret_key_len,
            public_key.as_mut_ptr(),
            &mut public_key_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(secret_key_len <= secret_key.capacity());
        debug_assert!(public_key_len <= public_key.capacity());
        secret_key.set_len(secret_key_len as usize);
        public_key.set_len(public_key_len as usize);
    }

    let secret_key = RsaSecretKey::from_vec(secret_key);
    let public_key = RsaPublicKey::from_vec(public_key);
    Ok(RsaKeyPair::join(secret_key, public_key))
}

/// Generates a pair of Elliptic Curve (ECDSA) keys.
///
/// Elliptic Curve keys are supported by both Secure Message and Secure Session objects.
///
/// # Panics
///
/// This function may panic in case of unrecoverable errors inside the library (e.g., out-of-memory
/// or assertion violations).
pub fn gen_ec_key_pair() -> EcdsaKeyPair {
    match try_gen_ec_key_pair() {
        Ok(keys) => keys,
        Err(e) => panic!("themis_gen_ec_key_pair() failed: {}", e),
    }
}

/// Generates a secret-public pair of ECDSA keys.
fn try_gen_ec_key_pair() -> Result<EcdsaKeyPair> {
    let mut secret_key = Vec::new();
    let mut public_key = Vec::new();
    let mut secret_key_len = 0;
    let mut public_key_len = 0;

    unsafe {
        let status = themis_gen_ec_key_pair(
            ptr::null_mut(),
            &mut secret_key_len,
            ptr::null_mut(),
            &mut public_key_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    secret_key.reserve(secret_key_len);
    public_key.reserve(secret_key_len);

    unsafe {
        let status = themis_gen_ec_key_pair(
            secret_key.as_mut_ptr(),
            &mut secret_key_len,
            public_key.as_mut_ptr(),
            &mut public_key_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(secret_key_len <= secret_key.capacity());
        debug_assert!(public_key_len <= public_key.capacity());
        secret_key.set_len(secret_key_len as usize);
        public_key.set_len(public_key_len as usize);
    }

    let secret_key = EcdsaSecretKey::from_vec(secret_key);
    let public_key = EcdsaPublicKey::from_vec(public_key);
    Ok(EcdsaKeyPair::join(secret_key, public_key))
}
