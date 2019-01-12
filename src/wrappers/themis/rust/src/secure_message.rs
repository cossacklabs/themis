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

//! Secure Message service.
//!
//! **Secure Message** is a lightweight service that can help deliver some message or data
//! to your peer in a secure manner.

use std::ptr;

use bindings::{themis_secure_message_unwrap, themis_secure_message_wrap};
use error::{Error, ErrorKind, Result};
use keys::{KeyPair, PublicKey, SecretKey};
use utils::into_raw_parts;

/// Secure Message encryption.
///
/// Messages produced by this object will be encrypted and verified for integrity.
#[derive(Clone)]
pub struct SecureMessage {
    key_pair: KeyPair,
}

impl SecureMessage {
    /// Makes a new Secure Message using given key pair.
    pub fn new<K: Into<KeyPair>>(key_pair: K) -> Self {
        Self {
            key_pair: key_pair.into(),
        }
    }

    /// Wraps the provided message into a secure encrypted message.
    pub fn wrap<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        wrap(
            self.key_pair.secret_key_bytes(),
            self.key_pair.public_key_bytes(),
            message.as_ref(),
        )
    }

    /// Unwraps an encrypted message back into its original form.
    pub fn unwrap<M: AsRef<[u8]>>(&self, wrapped: M) -> Result<Vec<u8>> {
        unwrap(
            self.key_pair.secret_key_bytes(),
            self.key_pair.public_key_bytes(),
            wrapped.as_ref(),
        )
    }
}

/// Secure Message signing.
///
/// Messages produced by this object will be signed and verified for integrity, but not encrypted.
///
/// The signatures can be checked with [`SecureVerify`].
///
/// [`SecureVerify`]: struct.SecureVerify.html
#[derive(Clone)]
pub struct SecureSign {
    secret_key: SecretKey,
}

impl SecureSign {
    /// Makes a new Secure Message using given secret key.
    pub fn new<S: Into<SecretKey>>(secret_key: S) -> Self {
        Self {
            secret_key: secret_key.into(),
        }
    }

    /// Securely signs a message and returns it with signature attached.
    pub fn sign<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        wrap(self.secret_key.as_ref(), &[], message.as_ref())
    }
}

/// Secure Message verification.
///
/// Verifies signatures produced by [`SecureSign`].
///
/// [`SecureSign`]: struct.SecureSign.html
#[derive(Clone)]
pub struct SecureVerify {
    public_key: PublicKey,
}

impl SecureVerify {
    /// Makes a new Secure Message using given public key.
    pub fn new<P: Into<PublicKey>>(public_key: P) -> Self {
        Self {
            public_key: public_key.into(),
        }
    }

    /// Verifies a signature and returns the original message.
    pub fn verify<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        unwrap(&[], self.public_key.as_ref(), message.as_ref())
    }
}

/// Wrap a message into a secure message.
fn wrap(secret_key: &[u8], public_key: &[u8], message: &[u8]) -> Result<Vec<u8>> {
    let (secret_key_ptr, secret_key_len) = into_raw_parts(secret_key);
    let (public_key_ptr, public_key_len) = into_raw_parts(public_key);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut wrapped = Vec::new();
    let mut wrapped_len = 0;

    unsafe {
        let status = themis_secure_message_wrap(
            secret_key_ptr,
            secret_key_len,
            public_key_ptr,
            public_key_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut wrapped_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    wrapped.reserve(wrapped_len);

    unsafe {
        let status = themis_secure_message_wrap(
            secret_key_ptr,
            secret_key_len,
            public_key_ptr,
            public_key_len,
            message_ptr,
            message_len,
            wrapped.as_mut_ptr(),
            &mut wrapped_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(wrapped_len <= wrapped.capacity());
        wrapped.set_len(wrapped_len as usize);
    }

    Ok(wrapped)
}

/// Unwrap a secure message into a message.
fn unwrap(secret_key: &[u8], public_key: &[u8], wrapped: &[u8]) -> Result<Vec<u8>> {
    let (secret_key_ptr, secret_key_len) = into_raw_parts(secret_key);
    let (public_key_ptr, public_key_len) = into_raw_parts(public_key);
    let (wrapped_ptr, wrapped_len) = into_raw_parts(wrapped);

    let mut message = Vec::new();
    let mut message_len = 0;

    unsafe {
        let status = themis_secure_message_unwrap(
            secret_key_ptr,
            secret_key_len,
            public_key_ptr,
            public_key_len,
            wrapped_ptr,
            wrapped_len,
            ptr::null_mut(),
            &mut message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    message.reserve(message_len);

    unsafe {
        let status = themis_secure_message_unwrap(
            secret_key_ptr,
            secret_key_len,
            public_key_ptr,
            public_key_len,
            wrapped_ptr,
            wrapped_len,
            message.as_mut_ptr(),
            &mut message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(message_len <= message.capacity());
        message.set_len(message_len as usize);
    }

    Ok(message)
}
