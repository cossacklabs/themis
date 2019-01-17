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

//! Secure Message system.
//!
//! **Secure Message** provides a sequence-independent, stateless, contextless messaging system.
//! This may be preferred in cases that do not require frequent sequential message exchange and/or
//! in low-bandwidth contexts. It is secure enough to exchange messages from time to time, but if
//! you would like to have Perfect Forward Secrecy and higher security guarantees, please consider
//! using [Secure Session] instead.
//!
//! Secure Message offers two modes of operation:
//!
//!   - In **[Sign]/[Verify]** mode the message is signed using the private key
//!     of the sender and is verified by the receiver using the public key of
//!     the sender. The message is packed in a suitable container and ECDSA is
//!     used by default to sign the message (when RSA key is used, RSA+PSS+PKCS#7
//!     digital signature is used).
//!
//!   - In **[Encrypt/Decrypt]** mode the message will be encrypted with a randomly
//!     generated key (in RSA) or a key derived by ECDH (in ECDSA), via symmetric
//!     algorithm with Secure Cell in seal mode (keys are 256 bits long).
//!
//! [Here you can read more][wiki] about cryptographic internals of Secure Messages.
//!
//! [Secure Session]: ../secure_session/index.html
//! [Sign]: struct.SecureSign.html
//! [Verify]: struct.SecureVerify.html
//! [Encrypt/Decrypt]: struct.SecureMessage.html
//! [wiki]: https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem
//!
//! # Examples
//!
//! Basic operation of Secure Message looks like this:
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! use themis::secure_message::SecureMessage;
//! use themis::keygen::gen_ec_key_pair;
//!
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
//!
//! You can find more examples for each operation mode in their respective documentation.

use std::ptr;

use bindings::{themis_secure_message_unwrap, themis_secure_message_wrap};

use crate::error::{Error, ErrorKind, Result};
use crate::keys::{KeyPair, PublicKey, SecretKey};
use crate::utils::into_raw_parts;

/// Secure Message encryption and decryption.
///
/// **Encrypted message** is useful when you need the full stack of protection for your data —
/// in most cases you will be using this flavor. Encrypted messages currently use Secure Cell
/// in sealing mode for data protection.
///
/// # Examples
///
/// In order to use Secure Message in encrypting mode you will need to have both public and
/// secret keys available on both peers. Typical usage of Secure Message looks like this:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureMessage;
/// use themis::keygen::gen_ec_key_pair;
///
/// // Generate and share this key pair between peers.
/// let key_pair = gen_ec_key_pair();
///
/// // Alice uses her own Secure Message instance to wrap (encrypt) messages.
/// let secure_a = SecureMessage::new(key_pair.clone());
/// let encrypted = secure_a.wrap(b"message")?;
///
/// // Bob uses his Secure Message instance to unwrap (decrypt) received messages.
/// let secure_b = SecureMessage::new(key_pair.clone());
/// let decrypted = secure_b.unwrap(&encrypted)?;
///
/// assert_eq!(decrypted, b"message");
/// # Ok(())
/// # }
/// ```
#[derive(Clone)]
pub struct SecureMessage {
    key_pair: KeyPair,
}

impl SecureMessage {
    /// Makes a new Secure Message using given key pair.
    ///
    /// Both ECDSA and RSA key pairs are supported.
    pub fn new<K: Into<KeyPair>>(key_pair: K) -> Self {
        Self {
            key_pair: key_pair.into(),
        }
    }

    /// Wraps the provided message into a secure encrypted message.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message: a byte slice or an array,
    /// a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_message::SecureMessage;
    /// use themis::keygen::gen_ec_key_pair;
    ///
    /// let secure = SecureMessage::new(gen_ec_key_pair());
    ///
    /// secure.wrap(b"byte string")?;
    /// secure.wrap(&[1, 2, 3, 4, 5])?;
    /// secure.wrap(vec![6, 7, 8, 9])?;
    /// secure.wrap(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
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
/// **Signed message** is useful for cases where you don’t need data confidentiality. It allows
/// the receiver to verify the origin and integrity of the data while still allowing intermediate
/// nodes to process it accordingly (for example, route data based on its type).
///
/// Signatures can be checked with [`SecureVerify`].
///
/// [`SecureVerify`]: struct.SecureVerify.html
///
/// # Examples
///
/// In order to sign messages you need only the secret part of a key pair. It does not need to be
/// shared with your peer for verification.
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureSign;
/// use themis::secure_message::SecureVerify;
/// use themis::keygen::gen_rsa_key_pair;
///
/// // Alice generates a key pair and shares `public` part with Bob
/// let (secret, public) = gen_rsa_key_pair().split();
///
/// // Alice is able to sign her messages with her secret key.
/// let secure_a = SecureSign::new(secret);
/// let signed_message = secure_a.sign(b"important message")?;
///
/// // Bob is able to verify that signature on the message matches.
/// let secure_b = SecureVerify::new(public);
/// let received_message = secure_b.verify(&signed_message)?;
/// assert_eq!(received_message, b"important message");
/// # Ok(())
/// # }
/// ```
///
/// Note that the signed message is _not encrypted_ and contains the original data as plain text:
///
/// ```
/// # use themis::secure_message::SecureSign;
/// # use themis::secure_message::SecureVerify;
/// # use themis::keygen::gen_rsa_key_pair;
/// #
/// # let (secret, _) = gen_rsa_key_pair().split();
/// # let secure = SecureSign::new(secret);
/// # let signed_message = secure.sign(b"important message").unwrap();
/// #
/// let message = b"important message";
///
/// assert!(signed_message.windows(message.len()).any(|subslice| subslice == message));
/// ```
#[derive(Clone)]
pub struct SecureSign {
    secret_key: SecretKey,
}

impl SecureSign {
    /// Makes a new Secure Message using given secret key.
    ///
    /// Both ECDSA and RSA keys are supported.
    pub fn new<S: Into<SecretKey>>(secret_key: S) -> Self {
        Self {
            secret_key: secret_key.into(),
        }
    }

    /// Securely signs a message and returns it with signature attached.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message: a byte slice or an array,
    /// a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_message::SecureSign;
    /// use themis::keygen::gen_ec_key_pair;
    ///
    /// let secure = SecureSign::new(gen_ec_key_pair().split().0);
    ///
    /// secure.sign(b"byte string")?;
    /// secure.sign(&[1, 2, 3, 4, 5])?;
    /// secure.sign(vec![6, 7, 8, 9])?;
    /// secure.sign(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
    pub fn sign<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        wrap(self.secret_key.as_ref(), &[], message.as_ref())
    }
}

// TODO: provide a way to inspect signed messages
//
// It would be nice to be able to get access to plaintext data in messages returned by SecureSign.
// Consider returning something like SignedMessage which is a newtype over Vec<u8> with additional
// utility methods and impls like AsRef<[u8]> and Into<Vec<u8>>.

/// Secure Message verification.
///
/// **Signed message** is useful for cases where you don’t need data confidentiality. It allows
/// the receiver to verify the origin and integrity of the data while still allowing intermediate
/// nodes to process it accordingly (for example, route data based on its type).
///
/// Verifies signatures produced by [`SecureSign`].
///
/// [`SecureSign`]: struct.SecureSign.html
///
/// # Examples
///
/// In order to verify signed messages you need the public part of a key pair corresponding to the
/// secret key used by your peer to sign messages.
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureSign;
/// use themis::secure_message::SecureVerify;
/// use themis::keygen::gen_ec_key_pair;
///
/// // Alice generates a key pair and shares `public` part with Bob
/// let (secret, public) = gen_ec_key_pair().split();
///
/// // Alice is able to sign her messages with her secret key.
/// let secure_a = SecureSign::new(secret);
/// let signed_message = secure_a.sign(b"important message")?;
///
/// // Bob is able to verify that signature on the message matches.
/// let secure_b = SecureVerify::new(public);
/// let received_message = secure_b.verify(&signed_message)?;
/// assert_eq!(received_message, b"important message");
/// # Ok(())
/// # }
/// ```
///
/// Secure Message guarantees integrity of the message and identity of its author.
///
/// ```
/// # use themis::secure_message::SecureSign;
/// # use themis::secure_message::SecureVerify;
/// # use themis::keygen::gen_ec_key_pair;
/// #
/// # let (secret, public) = gen_ec_key_pair().split();
/// # let secure_a = SecureSign::new(secret);
/// # let secure_b = SecureVerify::new(public);
/// # let signed_message = secure_a.sign(b"important message").unwrap();
/// #
/// // Let's flip some bits somewhere.
/// let mut corrupted_message = signed_message.clone();
/// corrupted_message[20] = !corrupted_message[20];
///
/// // Bob is able to see that the message has been tampered.
/// assert!(secure_b.verify(&corrupted_message).is_err());
///
/// // Only Alice's public key verifies the message, any other key won't do.
/// let (_, carol_public_key) = gen_ec_key_pair().split();
/// let secure_c = SecureVerify::new(carol_public_key);
///
/// assert!(secure_c.verify(&signed_message).is_err());
/// ```
#[derive(Clone)]
pub struct SecureVerify {
    public_key: PublicKey,
}

impl SecureVerify {
    /// Makes a new Secure Message using given public key.
    ///
    /// Both ECDSA and RSA keys are supported.
    pub fn new<P: Into<PublicKey>>(public_key: P) -> Self {
        Self {
            public_key: public_key.into(),
        }
    }

    /// Verifies the signature and returns the original message.
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
