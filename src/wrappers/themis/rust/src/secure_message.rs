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
//! [Here you can read more][docs] about cryptographic internals of Secure Messages.
//!
//! [Secure Session]: ../secure_session/index.html
//! [Sign]: struct.SecureSign.html
//! [Verify]: struct.SecureVerify.html
//! [Encrypt/Decrypt]: struct.SecureMessage.html
//! [docs]: https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-message/
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
//! let encrypted = secure.encrypt(b"message")?;
//! let decrypted = secure.decrypt(&encrypted)?;
//! assert_eq!(decrypted, b"message");
//! # Ok(())
//! # }
//! ```
//!
//! You can find more examples for each operation mode in their respective documentation.

use std::ptr;

use bindings::{
    themis_secure_message_decrypt, themis_secure_message_encrypt, themis_secure_message_sign,
    themis_secure_message_verify,
};

use crate::error::{Error, ErrorKind, Result};
use crate::keys::{KeyPair, PrivateKey, PublicKey};
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
/// private keys available on both peers. Typical usage of Secure Message looks like this:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureMessage;
/// use themis::keygen::gen_ec_key_pair;
///
/// // Generate and share this key pair between peers.
/// let key_pair = gen_ec_key_pair();
///
/// // Alice uses her own Secure Message instance to encrypt messages.
/// let secure_a = SecureMessage::new(key_pair.clone());
/// let encrypted = secure_a.encrypt(b"message")?;
///
/// // Bob uses his Secure Message instance to decrypt received messages.
/// let secure_b = SecureMessage::new(key_pair.clone());
/// let decrypted = secure_b.decrypt(&encrypted)?;
///
/// assert_eq!(decrypted, b"message");
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureMessage {
    key_pair: KeyPair,
}

impl SecureMessage {
    /// Makes a new Secure Message using given key pair.
    ///
    /// Both ECDSA and RSA key pairs are supported.
    pub fn new(key_pair: impl Into<KeyPair>) -> Self {
        Self {
            key_pair: key_pair.into(),
        }
    }

    /// Encrypts the provided message into a secure container.
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
    /// secure.encrypt(b"byte string")?;
    /// secure.encrypt(&[1, 2, 3, 4, 5])?;
    /// secure.encrypt(vec![6, 7, 8, 9])?;
    /// secure.encrypt(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (private_key_ptr, private_key_len) = into_raw_parts(self.key_pair.private_key_bytes());
        let (public_key_ptr, public_key_len) = into_raw_parts(self.key_pair.public_key_bytes());
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        let mut encrypted = Vec::new();
        let mut encrypted_len = 0;

        unsafe {
            let status = themis_secure_message_encrypt(
                private_key_ptr,
                private_key_len,
                public_key_ptr,
                public_key_len,
                message_ptr,
                message_len,
                ptr::null_mut(),
                &mut encrypted_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        encrypted.reserve(encrypted_len);

        unsafe {
            let status = themis_secure_message_encrypt(
                private_key_ptr,
                private_key_len,
                public_key_ptr,
                public_key_len,
                message_ptr,
                message_len,
                encrypted.as_mut_ptr(),
                &mut encrypted_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(encrypted_len <= encrypted.capacity());
            encrypted.set_len(encrypted_len);
        }

        Ok(encrypted)
    }

    /// Decrypts an encrypted message back into its original form.
    pub fn decrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (private_key_ptr, private_key_len) = into_raw_parts(self.key_pair.private_key_bytes());
        let (public_key_ptr, public_key_len) = into_raw_parts(self.key_pair.public_key_bytes());
        let (wrapped_ptr, wrapped_len) = into_raw_parts(message.as_ref());

        let mut decrypted = Vec::new();
        let mut decrypted_len = 0;

        unsafe {
            let status = themis_secure_message_decrypt(
                private_key_ptr,
                private_key_len,
                public_key_ptr,
                public_key_len,
                wrapped_ptr,
                wrapped_len,
                ptr::null_mut(),
                &mut decrypted_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        decrypted.reserve(decrypted_len);

        unsafe {
            let status = themis_secure_message_decrypt(
                private_key_ptr,
                private_key_len,
                public_key_ptr,
                public_key_len,
                wrapped_ptr,
                wrapped_len,
                decrypted.as_mut_ptr(),
                &mut decrypted_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(decrypted_len <= decrypted.capacity());
            decrypted.set_len(decrypted_len);
        }

        Ok(decrypted)
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
/// In order to sign messages you need only the private half of a key pair. It does not need to be
/// shared with your peer for verification.
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureSign;
/// use themis::secure_message::SecureVerify;
/// use themis::keygen::gen_rsa_key_pair;
///
/// // Alice generates a key pair and shares `public` half with Bob
/// let (private, public) = gen_rsa_key_pair().split();
///
/// // Alice is able to sign her messages with her private key.
/// let secure_a = SecureSign::new(private);
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
/// # let (private, _) = gen_rsa_key_pair().split();
/// # let secure = SecureSign::new(private);
/// # let signed_message = secure.sign(b"important message").unwrap();
/// #
/// let message = b"important message";
///
/// assert!(signed_message.windows(message.len()).any(|subslice| subslice == message));
/// ```
#[derive(Debug)]
pub struct SecureSign {
    private_key: PrivateKey,
}

impl SecureSign {
    /// Makes a new Secure Message using given private key.
    ///
    /// Both ECDSA and RSA keys are supported.
    pub fn new(private_key: impl Into<PrivateKey>) -> Self {
        Self {
            private_key: private_key.into(),
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
    pub fn sign(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (private_key_ptr, private_key_len) = into_raw_parts(self.private_key.as_ref());
        let (message_ptr, message_len) = into_raw_parts(message.as_ref());

        let mut signed = Vec::new();
        let mut signed_len = 0;

        unsafe {
            let status = themis_secure_message_sign(
                private_key_ptr,
                private_key_len,
                message_ptr,
                message_len,
                ptr::null_mut(),
                &mut signed_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        signed.reserve(signed_len);

        unsafe {
            let status = themis_secure_message_sign(
                private_key_ptr,
                private_key_len,
                message_ptr,
                message_len,
                signed.as_mut_ptr(),
                &mut signed_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(signed_len <= signed.capacity());
            signed.set_len(signed_len);
        }

        Ok(signed)
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
/// In order to verify signed messages you need the public half of a key pair corresponding to the
/// private key used by your peer to sign messages.
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_message::SecureSign;
/// use themis::secure_message::SecureVerify;
/// use themis::keygen::gen_ec_key_pair;
///
/// // Alice generates a key pair and shares `public` half with Bob
/// let (private, public) = gen_ec_key_pair().split();
///
/// // Alice is able to sign her messages with her private key.
/// let secure_a = SecureSign::new(private);
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
/// # let (private, public) = gen_ec_key_pair().split();
/// # let secure_a = SecureSign::new(private);
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
#[derive(Debug)]
pub struct SecureVerify {
    public_key: PublicKey,
}

impl SecureVerify {
    /// Makes a new Secure Message using given public key.
    ///
    /// Both ECDSA and RSA keys are supported.
    pub fn new(public_key: impl Into<PublicKey>) -> Self {
        Self {
            public_key: public_key.into(),
        }
    }

    /// Verifies the signature and returns the original message.
    pub fn verify(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        let (public_key_ptr, public_key_len) = into_raw_parts(self.public_key.as_ref());
        let (signed_ptr, signed_len) = into_raw_parts(message.as_ref());

        let mut original = Vec::new();
        let mut original_len = 0;

        unsafe {
            let status = themis_secure_message_verify(
                public_key_ptr,
                public_key_len,
                signed_ptr,
                signed_len,
                ptr::null_mut(),
                &mut original_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        original.reserve(original_len);

        unsafe {
            let status = themis_secure_message_verify(
                public_key_ptr,
                public_key_len,
                signed_ptr,
                signed_len,
                original.as_mut_ptr(),
                &mut original_len,
            );
            let error = Error::from_themis_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
            debug_assert!(original_len <= original.capacity());
            original.set_len(original_len);
        }

        Ok(original)
    }
}
