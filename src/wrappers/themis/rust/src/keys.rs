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

//! Cryptographic keys.
//!
//! This module contains data structures for keys supported by Themis: RSA and ECDSA key pairs.

use std::fmt;

use bindings::{themis_get_key_kind, themis_is_valid_key};
use error::{Error, ErrorKind, Result};
use utils::into_raw_parts;

/// Key material.
#[derive(Clone, Eq, PartialEq, Hash)]
pub(crate) struct KeyBytes(Vec<u8>);

// TODO: securely zero memory when dropping KeyBytes (?)

impl KeyBytes {
    /// Makes a key from an owned byte vector.
    pub fn from_vec(bytes: Vec<u8>) -> KeyBytes {
        KeyBytes(bytes)
    }

    /// Makes a key from a copy of a byte slice.
    pub fn copy_slice(bytes: &[u8]) -> KeyBytes {
        KeyBytes(bytes.to_vec())
    }

    /// Makes an empty key.
    pub fn empty() -> KeyBytes {
        KeyBytes(Vec::new())
    }

    /// Returns key bytes.
    pub fn as_bytes(&self) -> &[u8] {
        &self.0
    }
}

impl fmt::Debug for KeyBytes {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "KeyBytes({} bytes)", self.0.len())
    }
}

//
// Key type definitions
//

/// RSA secret key.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct RsaSecretKey {
    inner: KeyBytes,
}

/// RSA public key.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct RsaPublicKey {
    inner: KeyBytes,
}

/// RSA key pair.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct RsaKeyPair {
    secret_key: KeyBytes,
    public_key: KeyBytes,
}

/// ECDSA secret key.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct EcdsaSecretKey {
    inner: KeyBytes,
}

/// ECDSA public key.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct EcdsaPublicKey {
    inner: KeyBytes,
}

/// ECDSA key pair.
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct EcdsaKeyPair {
    secret_key: KeyBytes,
    public_key: KeyBytes,
}

/// A secret key.
///
/// This structure is used by cryptographic services which can support any kind of key.
/// [`RsaSecretKey`] or [`EcdsaSecretKey`] can be turned into a `SecretKey` at no cost.
///
/// [`RsaSecretKey`]: struct.RsaSecretKey.html
/// [`EcdsaSecretKey`]: struct.EcdsaSecretKey.html
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct SecretKey {
    inner: KeyBytes,
}

/// A public key.
///
/// This structure is used by cryptographic services which can support any kind of key.
/// [`RsaPublicKey`] or [`EcdsaPublicKey`] can be turned into a `PublicKey` at no cost.
///
/// [`RsaPublicKey`]: struct.RsaPublicKey.html
/// [`EcdsaPublicKey`]: struct.EcdsaPublicKey.html
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct PublicKey {
    inner: KeyBytes,
}

/// A pair of asymmetric keys.
///
/// This structure is used by cryptographic services which can support any kind of key pair.
/// [`RsaKeyPair`] or [`EcdsaKeyPair`] can be turned into a `KeyPair` at no cost. A pair of
/// [`SecretKey`] and [`PublicKey`] can be joined into a `KeyPair` after a quick type check
/// if their kinds match (either RSA or ECDSA).
///
/// [`RsaKeyPair`]: struct.RsaKeyPair.html
/// [`EcdsaKeyPair`]: struct.EcdsaKeyPair.html
/// [`SecretKey`]: struct.SecretKey.html
/// [`PublicKey`]: struct.PublicKey.html
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct KeyPair {
    secret_key: KeyBytes,
    public_key: KeyBytes,
}

/// Kind of an asymmetric key.
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum KeyKind {
    /// RSA secret key.
    RsaSecret,
    /// RSA public key.
    RsaPublic,
    /// ECDSA secret key.
    EcdsaSecret,
    /// ECDSA public key.
    EcdsaPublic,
}

//
// Key pairs
//

impl RsaKeyPair {
    /// Splits this key pair into secret and public keys.
    pub fn split(self) -> (RsaSecretKey, RsaPublicKey) {
        (
            RsaSecretKey {
                inner: self.secret_key,
            },
            RsaPublicKey {
                inner: self.public_key,
            },
        )
    }

    /// Joins a pair of secret and public keys.
    ///
    /// Note that this method _does not_ verify that the keys match: i.e., that it is possible
    /// to use the secret key to decrypt data encrypted with the public key.
    pub fn join(secret_key: RsaSecretKey, public_key: RsaPublicKey) -> RsaKeyPair {
        RsaKeyPair {
            secret_key: secret_key.inner,
            public_key: public_key.inner,
        }
    }
}

impl EcdsaKeyPair {
    /// Splits this key pair into secret and public keys.
    pub fn split(self) -> (EcdsaSecretKey, EcdsaPublicKey) {
        (
            EcdsaSecretKey {
                inner: self.secret_key,
            },
            EcdsaPublicKey {
                inner: self.public_key,
            },
        )
    }

    /// Joins a pair of secret and public keys.
    ///
    /// Note that this method _does not_ verify that the keys match: i.e., that it is possible
    /// to use the secret key to decrypt data encrypted with the public key.
    pub fn join(secret_key: EcdsaSecretKey, public_key: EcdsaPublicKey) -> EcdsaKeyPair {
        EcdsaKeyPair {
            secret_key: secret_key.inner,
            public_key: public_key.inner,
        }
    }
}

impl KeyPair {
    /// Access bytes of the secret key.
    pub(crate) fn secret_key_bytes(&self) -> &[u8] {
        self.secret_key.as_bytes()
    }

    /// Access bytes of the public key.
    pub(crate) fn public_key_bytes(&self) -> &[u8] {
        self.public_key.as_bytes()
    }

    /// Splits this key pair into secret and public keys.
    pub fn split(self) -> (SecretKey, PublicKey) {
        (
            SecretKey {
                inner: self.secret_key,
            },
            PublicKey {
                inner: self.public_key,
            },
        )
    }

    /// Joins a pair of secret and public keys.
    ///
    /// Note that this method _does not_ verify that the keys match: i.e., that it is possible
    /// to use the secret key to decrypt data encrypted with the public key.
    ///
    /// However, it does verify that _the kinds_ of the keys match: i.e., that they are both
    /// either RSA or ECDSA keys. An error is returned if that's not the case. You can check
    /// the kind of the key beforehand via its `kind()` method.
    pub fn try_join<S, P>(secret_key: S, public_key: P) -> Result<KeyPair>
    where
        S: Into<SecretKey>,
        P: Into<PublicKey>,
    {
        let (secret_key, public_key) = (secret_key.into(), public_key.into());
        match (secret_key.kind(), public_key.kind()) {
            (KeyKind::RsaSecret, KeyKind::RsaPublic) => {}
            (KeyKind::EcdsaSecret, KeyKind::EcdsaPublic) => {}
            _ => {
                return Err(Error::with_kind(ErrorKind::InvalidParameter));
            }
        }
        Ok(KeyPair {
            secret_key: secret_key.inner,
            public_key: public_key.inner,
        })
    }
}

//
// Individual keys
//

impl RsaSecretKey {
    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid RSA secret key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::RsaSecret => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }

    /// Wraps an existing trusted byte vector into a key.
    pub(crate) fn from_vec(bytes: Vec<u8>) -> Self {
        let key = KeyBytes::from_vec(bytes);
        debug_assert_eq!(get_key_kind(&key), Ok(KeyKind::RsaSecret));
        Self { inner: key }
    }
}

impl RsaPublicKey {
    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid RSA public key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::RsaPublic => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }

    /// Wraps an existing trusted byte vector into a key.
    pub(crate) fn from_vec(bytes: Vec<u8>) -> Self {
        let key = KeyBytes::from_vec(bytes);
        debug_assert_eq!(get_key_kind(&key), Ok(KeyKind::RsaPublic));
        Self { inner: key }
    }
}

impl EcdsaSecretKey {
    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid ECDSA secret key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::EcdsaSecret => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }

    /// Wraps an existing trusted byte vector into a key.
    pub(crate) fn from_vec(bytes: Vec<u8>) -> Self {
        let key = KeyBytes::from_vec(bytes);
        debug_assert_eq!(get_key_kind(&key), Ok(KeyKind::EcdsaSecret));
        Self { inner: key }
    }
}

impl EcdsaPublicKey {
    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid ECDSA public key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::EcdsaPublic => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }

    /// Wraps an existing trusted byte vector into a key.
    pub(crate) fn from_vec(bytes: Vec<u8>) -> Self {
        let key = KeyBytes::from_vec(bytes);
        debug_assert_eq!(get_key_kind(&key), Ok(KeyKind::EcdsaPublic));
        Self { inner: key }
    }
}

impl SecretKey {
    /// Retrieves actual kind of the stored key.
    pub fn kind(&self) -> KeyKind {
        get_key_kind_trusted(&self.inner)
    }

    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid RSA or ECDSA secret key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::RsaSecret => Ok(Self { inner: key }),
            KeyKind::EcdsaSecret => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }
}

impl PublicKey {
    /// Retrieves actual kind of the stored key.
    pub fn kind(&self) -> KeyKind {
        get_key_kind_trusted(&self.inner)
    }

    /// Parses a key from a byte slice.
    ///
    /// Returns an error if the slice does not contain a valid RSA or ECDSA public key.
    pub fn try_from_slice<T: AsRef<[u8]>>(bytes: T) -> Result<Self> {
        let key = KeyBytes::copy_slice(bytes.as_ref());
        match get_key_kind(&key)? {
            KeyKind::RsaPublic => Ok(Self { inner: key }),
            KeyKind::EcdsaPublic => Ok(Self { inner: key }),
            _ => Err(Error::with_kind(ErrorKind::InvalidParameter)),
        }
    }
}

// The following functions have to be called in a particular sequence in order to be safe to use.
// That's why they are free functions, not methods of KeyBytes.
//
// You can call get_key_kind() on any byte slice. If you get an Ok result back then you can call
// get_key_kind_trusted() again on the very same byte slice to get the result faster.
//
// There's also a reason why they receive &KeyBytes, not just &[u8]. This is to maintain correct
// pointer alignment. See "libthemis-sys/src/wrapper.c" for details.

fn get_key_kind(key: &KeyBytes) -> Result<KeyKind> {
    is_valid_themis_key(key)?;
    try_get_key_kind(key)
}

fn get_key_kind_trusted(key: &KeyBytes) -> KeyKind {
    debug_assert!(is_valid_themis_key(key).is_ok());
    try_get_key_kind(key).expect("get_key_kind_trusted() called for invalid key")
}

fn is_valid_themis_key(key: &KeyBytes) -> Result<()> {
    let (ptr, len) = into_raw_parts(key.as_bytes());
    let status = unsafe { themis_is_valid_key(ptr, len) };
    let error = Error::from_themis_status(status);
    if error.kind() != ErrorKind::Success {
        return Err(error);
    }
    Ok(())
}

fn try_get_key_kind(key: &KeyBytes) -> Result<KeyKind> {
    use bindings::themis_key_kind::*;
    let (ptr, len) = into_raw_parts(key.as_bytes());
    let kind = unsafe { themis_get_key_kind(ptr, len) };
    match kind {
        THEMIS_KEY_RSA_PRIVATE => Ok(KeyKind::RsaSecret),
        THEMIS_KEY_RSA_PUBLIC => Ok(KeyKind::RsaPublic),
        THEMIS_KEY_EC_PRIVATE => Ok(KeyKind::EcdsaSecret),
        THEMIS_KEY_EC_PUBLIC => Ok(KeyKind::EcdsaPublic),
        THEMIS_KEY_INVALID => Err(Error::with_kind(ErrorKind::InvalidParameter)),
    }
}

//
// AsRef<[u8]> casts
//

impl AsRef<[u8]> for RsaSecretKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

impl AsRef<[u8]> for RsaPublicKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

impl AsRef<[u8]> for EcdsaSecretKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

impl AsRef<[u8]> for EcdsaPublicKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

impl AsRef<[u8]> for SecretKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

impl AsRef<[u8]> for PublicKey {
    fn as_ref(&self) -> &[u8] {
        self.inner.as_bytes()
    }
}

//
// From/Into conversions
//

impl From<RsaSecretKey> for SecretKey {
    fn from(secret_key: RsaSecretKey) -> SecretKey {
        SecretKey {
            inner: secret_key.inner,
        }
    }
}

impl From<RsaPublicKey> for PublicKey {
    fn from(public_key: RsaPublicKey) -> PublicKey {
        PublicKey {
            inner: public_key.inner,
        }
    }
}

impl From<EcdsaSecretKey> for SecretKey {
    fn from(secret_key: EcdsaSecretKey) -> SecretKey {
        SecretKey {
            inner: secret_key.inner,
        }
    }
}

impl From<EcdsaPublicKey> for PublicKey {
    fn from(public_key: EcdsaPublicKey) -> PublicKey {
        PublicKey {
            inner: public_key.inner,
        }
    }
}

impl From<RsaKeyPair> for KeyPair {
    fn from(key_pair: RsaKeyPair) -> KeyPair {
        KeyPair {
            secret_key: key_pair.secret_key,
            public_key: key_pair.public_key,
        }
    }
}

impl From<EcdsaKeyPair> for KeyPair {
    fn from(key_pair: EcdsaKeyPair) -> KeyPair {
        KeyPair {
            secret_key: key_pair.secret_key,
            public_key: key_pair.public_key,
        }
    }
}
