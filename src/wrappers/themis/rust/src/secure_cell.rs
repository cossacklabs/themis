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

//! Secure Cell for data storage.
//!
//! **Secure Сell** is a high-level cryptographic service aimed at protecting arbitrary data
//! stored in various types of storage (e.g., databases, filesystem files, document archives,
//! cloud storage, etc.) It provides both strong symmetric encryption and data authentication
//! mechanism.
//!
//! The general approach is that given:
//!
//!   - _input:_ some source data to protect
//!   - _secret:_ symmetric key or a password
//!   - _context:_ and an optional “context information”
//!
//! Secure Cell will produce:
//!
//!   - _cell:_ the encrypted data
//!   - _authentication token:_ some authentication data
//!
//! The purpose of the optional context information (e.g., a database row number or file name)
//! is to establish a secure association between this context and the protected data.
//! In short, even when the secret is known, if the context is incorrect then decryption will fail.
//!
//! The purpose of the authentication data is to validate that given a correct key or passphrase
//! (and context), the decrypted data is indeed the same as the original source data.
//!
//! The authentication data must be stored somewhere. The most convenient way is to simply
//! append it to the encrypted data, but this is not always possible due to the storage
//! architecture of your application. Secure Cell offers variants that address this issue
//! in different ways.
//!
//! By default, Secure Cell uses AES-256 for encryption.
//! Authentication data takes additional 44 bytes when symmetric keys are used
//! and 70 bytes in case the data is secured with a passphrase.
//!
//! Secure Cell supports 2 kinds of secrets:
//!
//!   - **Symmetric keys** are convenient to store and efficient to use for machines.
//!     However, they are relatively long and hard for humans to remember.
//!
//!     New symmetric keys can be generated with [`SymmetricKey`].
//!
//!   - **Passphrases**, in contrast, can be shorter and easier to remember.
//!
//!     However, passphrases are typically much less random than keys.
//!     Secure Cell uses a *key derivation function* (KDF) to compensate for that
//!     and achieves security comparable to keys with shorter passphrases.
//!     This comes at a significant performance cost though.
//!
//! Use [`SecureCell::with_key`] or [`with_passphrase`] to construct a Secure Cell
//! with a particular kind of secret.
//!
//! [`SymmetricKey`]: ../keys/struct.SymmetricKey.html#method.new
//! [`SecureCell::with_key`]: struct.SecureCell.html#method.with_key
//! [`with_passphrase`]: struct.SecureCell.html#method.with_passphrase
//!
//! Secure Cell supports 3 operation modes:
//!
//!   - **[Seal mode]** is the most secure and easy to use.
//!     Your best choice most of the time.
//!     This is also the only mode that supports passphrases at the moment.
//!
//!   - **[Token Protect mode]** is just as secure, but a bit harder to use.
//!     This is your choice if you need to keep authentication data separate.
//!
//!   - **[Context Imprint mode]** is a length-preserving version of Secure Cell
//!     with no additional data stored. Should be used carefully.
//!
//! The operation mode is selected via an appropriate method of a basic [`SecureCell`] object.
//!
//! [Here you can learn more](https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/)
//! about the underlying considerations, limitations, and features of each mode.
//!
//! [`SecureCell`]: struct.SecureCell.html
//! [Seal mode]: struct.SecureCellSeal.html
//! [Token Protect mode]: struct.SecureCellTokenProtect.html
//! [Context Imprint mode]: struct.SecureCellContextImprint.html
//!
//! # Examples
//!
//! Here is how you use Secure Cell to seal away your data:
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! use themis::keys::SymmetricKey;
//! use themis::secure_cell::SecureCell;
//!
//! let key = SymmetricKey::new();
//! let cell = SecureCell::with_key(&key)?.seal();
//!
//! let encrypted = cell.encrypt(b"Beware the Jabberwock, my son!")?;
//! let decrypted = cell.decrypt(&encrypted)?;
//!
//! assert_eq!(decrypted, b"Beware the Jabberwock, my son!");
//! # Ok(())
//! # }
//! ```
//!
//! You can find more examples for each operation mode in their respective documentation.

use std::ptr;

use bindings::{
    themis_secure_cell_decrypt_context_imprint, themis_secure_cell_decrypt_seal,
    themis_secure_cell_decrypt_seal_with_passphrase, themis_secure_cell_decrypt_token_protect,
    themis_secure_cell_encrypt_context_imprint, themis_secure_cell_encrypt_seal,
    themis_secure_cell_encrypt_seal_with_passphrase, themis_secure_cell_encrypt_token_protect,
};

use crate::error::{Error, ErrorKind, Result};
use crate::keys::KeyBytes;
use crate::utils::into_raw_parts;

/// Basic Secure Cell.
///
/// This is modeless, basic cell constructed by [`SecureCell::with_key`] given a symmetric key.
/// After that you need to select the desired operation mode:
/// [`seal`], [`token_protect`], or [`context_imprint`].
///
/// You can also use passphrases instead of keys with [`SecureCell::with_passphrase`].
///
/// [`SecureCell::with_key`]: struct.SecureCell.html#method.with_key
/// [`SecureCell::with_passphrase`]: struct.SecureCell.html#method.with_passphrase
/// [`seal`]: struct.SecureCell.html#method.seal
/// [`token_protect`]: struct.SecureCell.html#method.token_protect
/// [`context_imprint`]: struct.SecureCell.html#method.context_imprint
#[derive(Debug)]
pub struct SecureCell {
    master_key: KeyBytes,
}

/// Basic Secure Cell with a passphrase.
///
/// This is modeless, basic cell constructed by [`SecureCell::with_passphrase`] given a passphrase.
/// Currently only Seal mode supports passphrases, it can be selected with [`seal`].
///
/// [`SecureCell::with_passphrase`]: struct.SecureCell.html#method.with_passphrase
/// [`seal`]: struct.SecureCellWithPassphrase.html#method.seal
#[derive(Debug)]
pub struct SecureCellWithPassphrase {
    passphrase: KeyBytes,
}

impl SecureCell {
    /// Constructs a new cell secured with a symmetric key.
    ///
    /// This method should be used with symmetric keys of sufficient length,
    /// such as generated by [`SymmetricKey`].
    ///
    /// If you have a human-readable passphrase, use [`SecureCell::with_passphrase`] instead.
    ///
    /// [`SymmetricKey`]: ../keys/struct.SymmetricKey.html#method.new
    /// [`SecureCell::with_passphrase`]: struct.SecureCell.html#method.with_passphrase
    ///
    /// # Examples
    ///
    /// The key may be provided as anything convertible into a byte slice,
    /// like an array, `Vec<u8>`, `bytes::Bytes`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), base64::DecodeError> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// assert!(SecureCell::with_key(SymmetricKey::new()).is_ok());
    ///
    /// let key = base64::decode("VGhhdCBiYXNlNjQgaW4gR29UaGVtaXMgd2FzIHRvbyBoYXJkLCBJIGd1ZXNz")?;
    /// assert!(SecureCell::with_key(&key).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Symmetric key cannot be empty.
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// assert!(SecureCell::with_key(&[]).is_err());
    /// ```
    pub fn with_key(symmetric_key: impl AsRef<[u8]>) -> Result<SecureCell> {
        Ok(SecureCell {
            master_key: KeyBytes::copy_slice(symmetric_key.as_ref())?,
        })
    }

    /// Constructs a new cell secured with a passphrase.
    ///
    /// You can safely use short, human-readable passphrases here.
    /// If you have a binary key, consider [`SecureCell::with_key`] instead.
    ///
    /// [`SecureCell::with_key`]: struct.SecureCell.html#method.with_key
    ///
    /// # Examples
    ///
    /// The passphrase is usually provided as a UTF-8 string (`&str` or `String`)
    /// for compatibility with other platforms.
    /// However, if you must use a different encoding,
    /// anything convertible into a byte slice is also accepted.
    ///
    /// ```
    /// use themis::secure_cell::SecureCell;
    ///
    /// assert!(SecureCell::with_passphrase("open sesame").is_ok());
    /// assert!(SecureCell::with_passphrase(format!("pretty please 🥺")).is_ok());
    ///
    /// // पासवर्ड in UTF-16BE
    /// let utf16_bytes = b"\x09\x2A\x09\x3E\x09\x38\x09\x35\x09\x30\x09\x4D\x09\x21";
    /// assert!(SecureCell::with_passphrase(utf16_bytes).is_ok());
    /// ```
    ///
    /// The passphrase cannot be empty.
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// assert!(SecureCell::with_passphrase("").is_err());
    /// ```
    pub fn with_passphrase(passphrase: impl AsRef<[u8]>) -> Result<SecureCellWithPassphrase> {
        Ok(SecureCellWithPassphrase {
            passphrase: KeyBytes::copy_slice(passphrase.as_ref())?,
        })
    }

    /// Switches this Secure Cell into the _Seal_ operation mode.
    pub fn seal(self) -> SecureCellSeal {
        SecureCellSeal(self)
    }

    /// Switches this Secure Cell into the _Token Protect_ operation mode.
    pub fn token_protect(self) -> SecureCellTokenProtect {
        SecureCellTokenProtect(self)
    }

    /// Switches this Secure Cell into the _Context Imprint_ operation mode.
    pub fn context_imprint(self) -> SecureCellContextImprint {
        SecureCellContextImprint(self)
    }
}

impl SecureCellWithPassphrase {
    /// Switches this Secure Cell into the _Seal_ operation mode.
    pub fn seal(self) -> SecureCellSealWithPassphrase {
        SecureCellSealWithPassphrase(self)
    }
}

/// Secure Cell in _Seal_ operation mode.
///
/// This is the most secure and easy way to protect stored data.
/// The data is protected by a symmetric key.
///
/// Secure Cell in Seal mode will encrypt the data and append an “authentication tag”
/// with auxiliary security information, forming a single sealed container.
/// This means that the encrypted data will be longer than the original input.
///
/// Additionally, it is possible to bind the encrypted data to some “associated context”
/// (for example, database row number).
/// In this case decryption of the data with incorrect context will fail
/// (even if the correct key is known and the data has not been tampered).
/// This establishes a cryptographically secure association between the protected data
/// and the context in which it is used.
/// With database row numbers this prevents the attacker from swapping encrypted password hashes
/// in the database so the system will not accept credentials of a different user.
///
/// Security of symmetric key operation mode depends on the quality of the key,
/// with short and incorrectly generated keys being easier to crack.
/// You can use [`SymmetricKey`] to generate good random keys of sufficient length.
/// If you need to use a short and easy to remember passphrase,
/// use passphrase API instead:
/// [`SecureCell::with_passphrase`] and [`SecureCellSealWithPassphrase`].
///
/// You can read more about Seal mode [in documentation][1].
///
/// [`SymmetricKey`]: ../keys/struct.SymmetricKey.html#method.new
/// [`SecureCell::with_passphrase`]: struct.SecureCell.html#method.with_passphrase
/// [`SecureCellSealWithPassphrase`]: struct.SecureCellSealWithPassphrase.html
/// [1]: https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#seal-mode
///
/// # Examples
///
/// Note that the resulting sealed cell takes more space than the input data:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::keys::SymmetricKey;
/// use themis::secure_cell::SecureCell;
///
/// let key = SymmetricKey::new();
/// let cell = SecureCell::with_key(&key)?.seal();
///
/// let plaintext = b"O frabjous day! Callooh! Callay!";
/// let encrypted = cell.encrypt(&plaintext)?;
/// let decrypted = cell.decrypt(&encrypted)?;
///
/// assert!(encrypted.len() > plaintext.len());
/// assert_eq!(decrypted, plaintext);
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureCellSeal(SecureCell);

impl SecureCellSeal {
    /// Encrypts the provided message.
    ///
    /// Data is encrypted and authentication token is appended to form a single sealed buffer.
    /// Use [`decrypt`] to decrypt the result later.
    ///
    /// This call is equivalent to [`encrypt_with_context`] with an empty associated context.
    ///
    /// [`decrypt`]: struct.SecureCellSeal.html#method.decrypt
    /// [`encrypt_with_context`]: struct.SecureCellSeal.html#method.encrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.seal();
    ///
    /// assert!(cell.encrypt(b"byte string").is_ok());
    /// assert!(cell.encrypt(&[1, 2, 3, 4, 5]).is_ok());
    /// assert!(cell.encrypt(vec![6, 7, 8, 9]).is_ok());
    /// assert!(cell.encrypt(format!("owned string")).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message must not be empty:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.encrypt_with_context(message, &[])
    }

    /// Encrypts the provided message with associated context.
    ///
    /// Data is encrypted and authentication token is appended to form a single sealed buffer.
    /// Use [`decrypt_with_context`] to decrypt the result later.
    ///
    /// The context is cryptographically mixed with the data
    /// but not included into the resulting encrypted message.
    /// You will have to provide the same context again during decryption.
    /// Usually this is some plaintext data associated with encrypted data,
    /// such as database row number, protocol message ID, etc.
    ///
    /// [`decrypt_with_context`]: struct.SecureCellSeal.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message and context:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.seal();
    ///
    /// assert!(cell.encrypt_with_context(b"byte string", format!("owned string")).is_ok());
    /// assert!(cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10]).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context may be empty (in which case this call is equivalent to [`encrypt`]).
    /// However, the message must not be empty.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", &[]).is_ok());
    /// assert!(cell.encrypt_with_context(&[], b"context").is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// [`encrypt`]: struct.SecureCellSeal.html#method.encrypt
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        encrypt_seal(
            self.0.master_key.as_bytes(),
            context.as_ref(),
            message.as_ref(),
        )
    }

    /// Decrypts the provided message.
    ///
    /// Secure Cell decrypts the message and verifies its integrity
    /// using authentication data embedded into the message.
    ///
    /// Use this method to decrypt data encrypted with [`encrypt`].
    /// If you use associated context, call [`decrypt_with_context`] instead.
    ///
    /// [`encrypt`]: struct.SecureCellSeal.html#method.encrypt
    /// [`decrypt_with_context`]: struct.SecureCellSeal.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// Obviously, the key must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.seal();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let encrypted = cell.encrypt(&message)?;
    /// let decrypted = cell.decrypt(&encrypted)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_key(SymmetricKey::new())?.seal();
    /// assert!(other_cell.decrypt(&encrypted).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// You also cannot use this method when data was encrypted with associated context.
    /// Use [`decrypt_with_context`] in that case.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let encrypted = cell.encrypt_with_context(&message, &context)?;
    ///
    /// assert!(cell.decrypt(&encrypted).is_err());
    ///
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    ///
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Seal mode verifies data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// # let encrypted = cell.encrypt(b"All mimsy were the borogoves")?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt(&corrupted).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.decrypt_with_context(message, &[])
    }

    /// Decrypts the provided message with associated context.
    ///
    /// Secure Cell validates association with the context data, decrypts the message,
    /// and verifies its integrity using authentication data embedded into the message.
    ///
    /// You need to provide the same context data as provided to [`encrypt_with_context`].
    /// You can also decrypt data encrypted with [`encrypt`] by using an empty context.
    ///
    /// [`encrypt_with_context`]: struct.SecureCellSeal.html#method.encrypt_with_context
    /// [`encrypt`]: struct.SecureCellSeal.html#method.encrypt
    ///
    /// # Examples
    ///
    /// Obviously, the key must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.seal();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_key(SymmetricKey::new())?.seal();
    /// assert!(other_cell.decrypt_with_context(&encrypted, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context must match as well for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// assert!(cell.decrypt_with_context(&encrypted, b"incorrect context").is_err());
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Seal mode verifies data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt_with_context(&corrupted, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_seal(
            self.0.master_key.as_bytes(),
            context.as_ref(),
            message.as_ref(),
        )
    }
}

/// Secure Cell in _Seal_ operation mode.
///
/// This is the most secure and easy way to protect stored data.
/// You can safely use short, human-readable passphrase strings in this mode.
///
/// Secure Cell in Seal mode will encrypt the data and append an “authentication tag”
/// with auxiliary security information, forming a single sealed container.
/// This means that the encrypted data will be larger than the original input.
///
/// Additionally, it is possible to bind the encrypted data to some “associated context”
/// (for example, database row number).
/// In this case decryption of the data with incorrect context will fail
/// (even if the passphrase is correct and the data has not been tampered).
/// This establishes a cryptographically secure association between the protected data
/// and the context in which it is used.
/// With database row numbers this prevents the attacker from swapping encrypted password hashes
/// in the database so the system will not accept credentials of a different user.
///
/// Passphrase operation mode uses a _key derivation function_ (KDF) for security
/// and because of that it is quite slow (10,000+ times slower than using keys directly).
/// If you don’t have to memorize the secret or when performance is important,
/// consider using symmetric key API:
/// [`SecureCell::with_key`] and [`SecureCellSeal`].
///
/// You can read more about Seal mode [in documentation][1].
///
/// [`SecureCell::with_key`]: struct.SecureCell.html#method.with_key
/// [`SecureCellSeal`]: struct.SecureCellSeal.html
/// [1]: https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#seal-mode
///
/// # Examples
///
/// Note that the resulting sealed cell takes more space than the input data:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_cell::SecureCell;
///
/// let cell = SecureCell::with_passphrase("open sesame")?.seal();
///
/// let plaintext = b"O frabjous day! Callooh! Callay!";
/// let encrypted = cell.encrypt(&plaintext)?;
/// let decrypted = cell.decrypt(&encrypted)?;
///
/// assert!(encrypted.len() > plaintext.len());
/// assert_eq!(decrypted, plaintext);
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureCellSealWithPassphrase(SecureCellWithPassphrase);

impl SecureCellSealWithPassphrase {
    /// Encrypts the provided message.
    ///
    /// Data is encrypted and authentication token is appended to form a single sealed buffer.
    /// Use [`decrypt`] to decrypt the result later.
    ///
    /// This call is equivalent to [`encrypt_with_context`] with an empty associated context.
    ///
    /// [`decrypt`]: struct.SecureCellSealWithPassphrase.html#method.decrypt
    /// [`encrypt_with_context`]: struct.SecureCellSealWithPassphrase.html#method.encrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_passphrase("open sesame")?.seal();
    ///
    /// assert!(cell.encrypt(b"byte string").is_ok());
    /// assert!(cell.encrypt(&[1, 2, 3, 4, 5]).is_ok());
    /// assert!(cell.encrypt(vec![6, 7, 8, 9]).is_ok());
    /// assert!(cell.encrypt(format!("owned string")).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message must not be empty:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.encrypt_with_context(message, &[])
    }

    /// Encrypts the provided message with associated context.
    ///
    /// Data is encrypted and authentication token is appended to form a single sealed buffer.
    /// Use [`decrypt_with_context`] to decrypt the result later.
    ///
    /// The context is cryptographically mixed with the data
    /// but not included into the resulting encrypted message.
    /// You will have to provide the same context again during decryption.
    /// Usually this is some plaintext data associated with encrypted data,
    /// such as database row number, protocol message ID, etc.
    ///
    /// [`decrypt_with_context`]: struct.SecureCellSealWithPassphrase.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message and context:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_passphrase("open sesame")?.seal();
    ///
    /// assert!(cell.encrypt_with_context(b"byte string", format!("owned string")).is_ok());
    /// assert!(cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10]).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context may be empty (in which case this call is equivalent to [`encrypt`]).
    /// However, the message must not be empty.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", &[]).is_ok());
    /// assert!(cell.encrypt_with_context(&[], b"context").is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// [`encrypt`]: struct.SecureCellSealWithPassphrase.html#method.encrypt
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        encrypt_seal_with_passphrase(
            self.0.passphrase.as_bytes(),
            context.as_ref(),
            message.as_ref(),
        )
    }

    /// Decrypts the provided message.
    ///
    /// Secure Cell decrypts the message and verifies its integrity
    /// using authentication data embedded into the message.
    ///
    /// Use this method to decrypt data encrypted with [`encrypt`].
    /// If you use associated context, call [`decrypt_with_context`] instead.
    ///
    /// [`encrypt`]: struct.SecureCellSealWithPassphrase.html#method.encrypt
    /// [`decrypt_with_context`]: struct.SecureCellSealWithPassphrase.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// Obviously, the passphrase must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_passphrase("open sesame")?.seal();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let encrypted = cell.encrypt(&message)?;
    /// let decrypted = cell.decrypt(&encrypted)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_passphrase("swordfish")?.seal();
    /// assert!(other_cell.decrypt(&encrypted).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// You also cannot use this method when data was encrypted with associated context.
    /// Use [`decrypt_with_context`] in that case.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let encrypted = cell.encrypt_with_context(&message, &context)?;
    ///
    /// assert!(cell.decrypt(&encrypted).is_err());
    ///
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    ///
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Seal mode verifies data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// # let encrypted = cell.encrypt(b"All mimsy were the borogoves")?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt(&corrupted).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.decrypt_with_context(message, &[])
    }

    /// Decrypts the provided message with associated context.
    ///
    /// Secure Cell validates association with the context data, decrypts the message,
    /// and verifies its integrity using authentication data embedded into the message.
    ///
    /// You need to provide the same context data as provided to [`encrypt_with_context`].
    /// You can also decrypt data encrypted with [`encrypt`] by using an empty context.
    ///
    /// [`encrypt_with_context`]: struct.SecureCellSealWithPassphrase.html#method.encrypt_with_context
    /// [`encrypt`]: struct.SecureCellSealWithPassphrase.html#method.encrypt
    ///
    /// # Examples
    ///
    /// Obviously, the passphrase must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_passphrase("open sesame")?.seal();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_passphrase(b"swordfish")?.seal();
    /// assert!(other_cell.decrypt_with_context(&encrypted, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context must match as well for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// assert!(cell.decrypt_with_context(&encrypted, b"incorrect context").is_err());
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Seal mode verifies data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_passphrase("open sesame")?.seal();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt_with_context(&corrupted, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_seal_with_passphrase(
            self.0.passphrase.as_bytes(),
            context.as_ref(),
            message.as_ref(),
        )
    }
}

/// Encrypts `message` with `master_key` including optional `user_context` for verification.
fn encrypt_seal(master_key: &[u8], user_context: &[u8], message: &[u8]) -> Result<Vec<u8>> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (user_context_ptr, user_context_len) = into_raw_parts(user_context);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut encrypted_message = Vec::new();
    let mut encrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_encrypt_seal(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    encrypted_message.reserve(encrypted_message_len);

    unsafe {
        let status = themis_secure_cell_encrypt_seal(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            encrypted_message.as_mut_ptr(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(encrypted_message_len <= encrypted_message.capacity());
        encrypted_message.set_len(encrypted_message_len);
    }

    Ok(encrypted_message)
}

/// Decrypts `message` with `master_key` and verifies authenticity of `user_context`.
fn decrypt_seal(master_key: &[u8], user_context: &[u8], message: &[u8]) -> Result<Vec<u8>> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (user_context_ptr, user_context_len) = into_raw_parts(user_context);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut decrypted_message = Vec::new();
    let mut decrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_decrypt_seal(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    decrypted_message.reserve(decrypted_message_len);

    unsafe {
        let status = themis_secure_cell_decrypt_seal(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            decrypted_message.as_mut_ptr(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(decrypted_message_len <= decrypted_message.capacity());
        decrypted_message.set_len(decrypted_message_len);
    }

    Ok(decrypted_message)
}

/// Encrypts `message` with `passphrase` including optional `context` for verification.
fn encrypt_seal_with_passphrase(
    passphrase: &[u8],
    context: &[u8],
    message: &[u8],
) -> Result<Vec<u8>> {
    let (passphrase_ptr, passphrase_len) = into_raw_parts(passphrase);
    let (context_ptr, context_len) = into_raw_parts(context);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut encrypted_message = Vec::new();
    let mut encrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_encrypt_seal_with_passphrase(
            passphrase_ptr,
            passphrase_len,
            context_ptr,
            context_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    encrypted_message.reserve(encrypted_message_len);

    unsafe {
        let status = themis_secure_cell_encrypt_seal_with_passphrase(
            passphrase_ptr,
            passphrase_len,
            context_ptr,
            context_len,
            message_ptr,
            message_len,
            encrypted_message.as_mut_ptr(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(encrypted_message_len <= encrypted_message.capacity());
        encrypted_message.set_len(encrypted_message_len);
    }

    Ok(encrypted_message)
}

/// Decrypts `message` with `passphrase` and verifies authenticity of `context`.
fn decrypt_seal_with_passphrase(
    passphrase: &[u8],
    context: &[u8],
    message: &[u8],
) -> Result<Vec<u8>> {
    let (passphrase_ptr, passphrase_len) = into_raw_parts(passphrase);
    let (context_ptr, context_len) = into_raw_parts(context);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut decrypted_message = Vec::new();
    let mut decrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_decrypt_seal_with_passphrase(
            passphrase_ptr,
            passphrase_len,
            context_ptr,
            context_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    decrypted_message.reserve(decrypted_message_len);

    unsafe {
        let status = themis_secure_cell_decrypt_seal_with_passphrase(
            passphrase_ptr,
            passphrase_len,
            context_ptr,
            context_len,
            message_ptr,
            message_len,
            decrypted_message.as_mut_ptr(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(decrypted_message_len <= decrypted_message.capacity());
        decrypted_message.set_len(decrypted_message_len);
    }

    Ok(decrypted_message)
}

/// Secure Cell in _Token Protect_ operation mode.
///
/// This is a modified Seal mode for constrained environments.
/// The data is protected by a symmetric key.
///
/// Token Protect mode is designed for cases when underlying storage constraints
/// do not allow the size of the data to grow (so [`SecureCellSeal`] cannot be used).
/// However, if you have access to a different storage location
/// (e.g., another table in the database) where additional security parameters can be stored
/// then Token Protect mode can be used instead of Seal mode.
///
/// [`SecureCellSeal`]: struct.SecureCellSeal.html
///
/// Token Protect mode produces authentication tag and other auxiliary data
/// (aka “authentication token”) in a detached buffer.
/// This keeps the original size of the encrypted data
/// while enabling separate storage of security information.
/// Note that the same token must be provided along with the correct secret
/// and matching associated context in order for the data to be decrypted successfully.
///
/// Since `SecureCellTokenProtect` uses the same security parameters as `SecureCellSeal`
/// (just stored in a different location), these modes have the same highest security level.
/// Token Protect mode only requires slightly more programming effort
/// in exchange for preserving the original data size.
///
/// Security of symmetric key operation mode depends on the quality of the key,
/// with short and incorrectly generated keys being easier to crack.
/// You can use [`SymmetricKey`] to generate good random keys of sufficient length.
///
/// You can read more about Token Protect mode [in documentation][1].
///
/// [`SymmetricKey`]: ../keys/struct.SymmetricKey.html#method.new
/// [1]: https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#token-protect-mode
///
/// # Examples
///
/// Note that the resulting encrypted data takes the same space as the input data
/// but there is an additional data token:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::keys::SymmetricKey;
/// use themis::secure_cell::SecureCell;
///
/// let key = SymmetricKey::new();
/// let cell = SecureCell::with_key(&key)?.token_protect();
///
/// let plaintext = b"O frabjous day! Callooh! Callay!";
/// let (encrypted, token) = cell.encrypt(&plaintext)?;
/// let decrypted = cell.decrypt(&encrypted, &token)?;
///
/// assert_eq!(decrypted, plaintext);
/// assert_eq!(encrypted.len(), plaintext.len());
/// assert_eq!(token.len(), 44);
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureCellTokenProtect(SecureCell);

impl SecureCellTokenProtect {
    /// Encrypts the provided message.
    ///
    /// Data is encrypted and authentication token is produced separately.
    /// Use [`decrypt`] to decrypt the result later,
    /// providing both encrypted data and corresponding token.
    ///
    /// This call is equivalent to [`encrypt_with_context`] with an empty associated context.
    ///
    /// [`decrypt`]: struct.SecureCellTokenProtect.html#method.decrypt
    /// [`encrypt_with_context`]: struct.SecureCellTokenProtect.html#method.encrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.token_protect();
    ///
    /// assert!(cell.encrypt(b"byte string").is_ok());
    /// assert!(cell.encrypt(&[1, 2, 3, 4, 5]).is_ok());
    /// assert!(cell.encrypt(vec![6, 7, 8, 9]).is_ok());
    /// assert!(cell.encrypt(format!("owned string")).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message must not be empty:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<(Vec<u8>, Vec<u8>)> {
        self.encrypt_with_context(message, &[])
    }

    /// Encrypts the provided message with associated context.
    ///
    /// Data is encrypted and authentication token is produced separately.
    /// Use [`decrypt_with_context`] to decrypt the result later,
    /// providing both encrypted data and corresponding token.
    ///
    /// The context is cryptographically mixed with the data
    /// but not included into the resulting encrypted message.
    /// You will have to provide the same context again during decryption.
    /// Usually this is some plaintext data associated with encrypted data,
    /// such as database row number, protocol message ID, etc.
    ///
    /// [`decrypt_with_context`]: struct.SecureCellTokenProtect.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message and context:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.token_protect();
    ///
    /// assert!(cell.encrypt_with_context(b"byte string", format!("owned string")).is_ok());
    /// assert!(cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10]).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context may be empty (in which case this call is equivalent to [`encrypt`]).
    /// However, the message must not be empty.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", &[]).is_ok());
    /// assert!(cell.encrypt_with_context(&[], b"context").is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// [`encrypt`]: struct.SecureCellTokenProtect.html#method.encrypt
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<(Vec<u8>, Vec<u8>)> {
        encrypt_token_protect(
            self.0.master_key.as_bytes(),
            context.as_ref(),
            message.as_ref(),
        )
    }

    /// Decrypts the provided message.
    ///
    /// Secure Cell decrypts the message and verifies its integrity
    /// using the provided authentication token.
    ///
    /// Use this method to decrypt data encrypted with [`encrypt`].
    /// If you use associated context, call [`decrypt_with_context`] instead.
    ///
    /// [`encrypt`]: struct.SecureCellTokenProtect.html#method.encrypt
    /// [`decrypt_with_context`]: struct.SecureCellTokenProtect.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// Obviously, the key must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.token_protect();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let (encrypted, token) = cell.encrypt(&message)?;
    /// let decrypted = cell.decrypt(&encrypted, &token)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_key(SymmetricKey::new())?.token_protect();
    /// assert!(other_cell.decrypt(&encrypted, &token).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The tokens must also match the encrypted messages, even if the original data is the same.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// #
    /// # let message = b"All mimsy were the borogoves";
    /// #
    /// let (encrypted,   _) = cell.encrypt(&message)?;
    /// let (_, other_token) = cell.encrypt(&message)?;
    ///
    /// assert!(cell.decrypt(&encrypted, &other_token).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// You also cannot use this method when data was encrypted with associated context.
    /// Use [`decrypt_with_context`] in that case.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// # let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let (encrypted, token) = cell.encrypt_with_context(&message, &context)?;
    ///
    /// assert!(cell.decrypt(&encrypted, &token).is_err());
    ///
    /// let decrypted = cell.decrypt_with_context(&encrypted, &token, &context)?;
    ///
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Token Protect mode verifies data integrity
    /// and can see if the data or token was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// # let (encrypted, auth_token) = cell.encrypt(b"All mimsy were the borogoves")?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted_data = encrypted.clone();
    /// let mut corrupted_token = auth_token.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    /// corrupted_token[9] = !corrupted_token[9];
    ///
    /// assert!(cell.decrypt(&corrupted_data, &auth_token).is_err());
    /// assert!(cell.decrypt(&encrypted, &corrupted_token).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt(&self, message: impl AsRef<[u8]>, token: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.decrypt_with_context(message, token, &[])
    }

    /// Decrypts the provided message with associated context.
    ///
    /// Secure Cell validates association with the context data, decrypts the message,
    /// and verifies its integrity using the provided authentication token.
    ///
    /// You need to provide the same context data as provided to [`encrypt_with_context`].
    /// You can also decrypt data encrypted with [`encrypt`] by using an empty context.
    ///
    /// [`encrypt_with_context`]: struct.SecureCellTokenProtect.html#method.encrypt_with_context
    /// [`encrypt`]: struct.SecureCellTokenProtect.html#method.encrypt
    ///
    /// # Examples
    ///
    /// Obviously, the key must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.token_protect();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let (encrypted, token) = cell.encrypt_with_context(&message, &context)?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, &token, &context)?;
    /// assert_eq!(decrypted, message);
    ///
    /// let other_cell = SecureCell::with_key(SymmetricKey::new())?.token_protect();
    /// assert!(other_cell.decrypt_with_context(&encrypted, &token, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The tokens must also match the encrypted messages, even if the original data is the same.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// #
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// #
    /// let (encrypted,   _) = cell.encrypt_with_context(&message, &context)?;
    /// let (_, other_token) = cell.encrypt_with_context(&message, &context)?;
    ///
    /// assert!(cell.decrypt_with_context(&encrypted, &other_token, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context must match as well for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let (encrypted, token) = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// assert!(cell.decrypt_with_context(&encrypted, &token, b"incorrect context").is_err());
    ///
    /// let decrypted = cell.decrypt_with_context(&encrypted, &token, &context)?;
    /// assert_eq!(decrypted, b"All mimsy were the borogoves");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in Token Protect mode verifies data integrity
    /// and can see if the data or token was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.token_protect();
    /// # let message = b"All mimsy were the borogoves";
    /// # let context = b"And the mome raths outgrabe";
    /// # let (encrypted, auth_token) = cell.encrypt_with_context(&message, &context)?;
    /// #
    /// // Let's flip some bits somewhere...
    /// let mut corrupted_data = encrypted.clone();
    /// let mut corrupted_token = auth_token.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    /// corrupted_token[9] = !corrupted_token[9];
    ///
    /// assert!(cell.decrypt_with_context(&corrupted_data, &auth_token, &context).is_err());
    /// assert!(cell.decrypt_with_context(&encrypted, &corrupted_token, &context).is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        token: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_token_protect(
            self.0.master_key.as_bytes(),
            context.as_ref(),
            message.as_ref(),
            token.as_ref(),
        )
    }
}

/// Encrypts `message` with `master_key` including optional `user_context` for verification.
/// Returns `(ciphertext, auth_token)` tuple.
fn encrypt_token_protect(
    master_key: &[u8],
    user_context: &[u8],
    message: &[u8],
) -> Result<(Vec<u8>, Vec<u8>)> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (user_context_ptr, user_context_len) = into_raw_parts(user_context);
    let (message_ptr, message_len) = into_raw_parts(message);

    let mut token = Vec::new();
    let mut token_len = 0;
    let mut encrypted_message = Vec::new();
    let mut encrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_encrypt_token_protect(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            ptr::null_mut(),
            &mut token_len,
            ptr::null_mut(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    token.reserve(token_len);
    encrypted_message.reserve(encrypted_message_len);

    unsafe {
        let status = themis_secure_cell_encrypt_token_protect(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            token.as_mut_ptr(),
            &mut token_len,
            encrypted_message.as_mut_ptr(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(token_len <= token.capacity());
        token.set_len(token_len);
        debug_assert!(encrypted_message_len <= encrypted_message.capacity());
        encrypted_message.set_len(encrypted_message_len);
    }

    Ok((encrypted_message, token))
}

/// Decrypts `message` with `master_key` and `token` verifying `user_context`.
fn decrypt_token_protect(
    master_key: &[u8],
    user_context: &[u8],
    message: &[u8],
    token: &[u8],
) -> Result<Vec<u8>> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (user_context_ptr, user_context_len) = into_raw_parts(user_context);
    let (message_ptr, message_len) = into_raw_parts(message);
    let (token_ptr, token_len) = into_raw_parts(token);

    let mut decrypted_message = Vec::new();
    let mut decrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_decrypt_token_protect(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            token_ptr,
            token_len,
            ptr::null_mut(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    decrypted_message.reserve(decrypted_message_len);

    unsafe {
        let status = themis_secure_cell_decrypt_token_protect(
            master_key_ptr,
            master_key_len,
            user_context_ptr,
            user_context_len,
            message_ptr,
            message_len,
            token_ptr,
            token_len,
            decrypted_message.as_mut_ptr(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(decrypted_message_len <= decrypted_message.capacity());
        decrypted_message.set_len(decrypted_message_len);
    }

    Ok(decrypted_message)
}

/// Secure Cell in _Context Imprint_ operation mode.
///
/// This is an advanced mode for constrained environments.
/// The data is protected by a symmetric key.
///
/// Context Imprint mode is intended for environments where storage constraints
/// do not allow the size of the data to grow and there is no auxiliary storage available.
/// Context Imprint mode requires an additional “associated context”
/// to be provided along with the key in order to protect the data.
///
/// In Context Imprint mode no authentication token is computed or verified.
/// This means that integrity of the data is not enforced,
/// so the overall security level is slightly lower than in Seal or Token Protect modes.
///
/// Security of symmetric key operation mode depends on the quality of the key,
/// with short and incorrectly generated keys being easier to crack.
/// You can use [`SymmetricKey`] to generate good random keys of sufficient length.
///
/// To ensure the highest security level possible,
/// supply a different associated context for each encryption invocation with the same key.
///
/// You can read more about Context Imprint mode [in documentation][1].
///
/// [`SymmetricKey`]: ../keys/struct.SymmetricKey.html#method.new
/// [1]: https://docs.cossacklabs.com/themis/crypto-theory/cryptosystems/secure-cell/#context-imprint-mode
///
/// # Examples
///
/// Note that the resulting encrypted data takes the same space as the input data:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::keys::SymmetricKey;
/// use themis::secure_cell::SecureCell;
///
/// let key = SymmetricKey::new();
/// let cell = SecureCell::with_key(&key)?.context_imprint();
///
/// let plaintext = b"O frabjous day! Callooh! Callay!";
/// let context = b"And the mome raths outgrabe";
///
/// let encrypted = cell.encrypt_with_context(&plaintext, &context)?;
/// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
///
/// assert_eq!(decrypted, plaintext);
/// assert_eq!(encrypted.len(), plaintext.len());
/// # Ok(())
/// # }
/// ```
///
/// In Context Imprint mode you *must* provide non-empty associated context.
/// Also keep in mind that Secure Cell cannot verify integrity of the decrypted data
/// so you should have some other means in place to validate the output.
#[derive(Debug)]
pub struct SecureCellContextImprint(SecureCell);

impl SecureCellContextImprint {
    /// Encrypts the provided message with associated context.
    ///
    /// Data is encrypted and combined with the provided context.
    /// Use [`decrypt_with_context`] to decrypt the result later.
    ///
    /// The context is cryptographically mixed with the data
    /// but not included into the resulting encrypted message.
    /// You will have to provide the same context again during decryption.
    /// Usually this is some plaintext data associated with encrypted data,
    /// such as database row number, protocol message ID, etc.
    ///
    /// [`decrypt_with_context`]: struct.SecureCellContextImprint.html#method.decrypt_with_context
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message and context:
    /// a byte slice or an array, a `Vec<u8>`, or a `String`, etc.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.context_imprint();
    ///
    /// assert!(cell.encrypt_with_context(b"byte string", format!("owned string")).is_ok());
    /// assert!(cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10]).is_ok());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message and context must not be empty.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let key = SymmetricKey::new();
    /// # let cell = SecureCell::with_key(&key)?.context_imprint();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", b"context").is_ok());
    /// assert!(cell.encrypt_with_context(b"",        b"context").is_err());
    /// assert!(cell.encrypt_with_context(b"message",        b"").is_err());
    /// # Ok(())
    /// # }
    /// ```
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        encrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            context.as_ref(),
        )
    }

    /// Decrypts the provided message with associated context.
    ///
    /// Secure Cell validates association with the context data and decrypts the message.
    /// You need to provide the same context data as provided to [`encrypt_with_context`].
    ///
    /// [`encrypt_with_context`]: struct.SecureCellContextImprint.html#method.encrypt_with_context
    ///
    /// Note that in Context Imprint mode messages do not include any authentication token
    /// for integrity validation, thus the returned message might not be the original one
    /// even if it has been decrypted ‘successfully’.
    ///
    /// # Examples
    ///
    /// Obviously, the key and context must be the same for decryption to succeed:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::keys::SymmetricKey;
    /// use themis::secure_cell::SecureCell;
    ///
    /// let key = SymmetricKey::new();
    /// let cell = SecureCell::with_key(&key)?.context_imprint();
    ///
    /// let message = b"All mimsy were the borogoves";
    /// let context = b"And the mome raths outgrabe";
    /// let encrypted = cell.encrypt_with_context(&message, &context)?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, &context)?;
    /// assert_eq!(decrypted, message);
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, Secure Cell in Context Imprint mode cannot verify
    /// that the message was decrypted correctly.
    /// If a different key is used or the associated context does not match,
    /// you typically get garbage output instead of an error.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// let cell1 = SecureCell::with_key(SymmetricKey::new())?.context_imprint();
    /// let cell2 = SecureCell::with_key(SymmetricKey::new())?.context_imprint();
    ///
    /// let encrypted = cell1.encrypt_with_context(b"Bandersnatch", b"Jabberwock")?;
    /// let decrypted = cell2.decrypt_with_context(&encrypted, b"the Jubjub bird")?;
    ///
    /// assert_ne!(decrypted, b"Bandersnatch");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Context Imprint mode is also unable to detect data corruption.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::keys::SymmetricKey;
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(SymmetricKey::new())?.context_imprint();
    /// # let encrypted = cell.encrypt_with_context(b"Bandersnatch", b"Jabberwock")?;
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted_data = encrypted.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    ///
    /// let result = cell.decrypt_with_context(&corrupted_data, b"Jabberwock");
    /// assert_ne!(result.expect("no verification"), b"Bandersnatch");
    /// # Ok(())
    /// # }
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            context.as_ref(),
        )
    }
}

/// Encrypts `message` with `master_key` including required `context`.
fn encrypt_context_imprint(master_key: &[u8], message: &[u8], context: &[u8]) -> Result<Vec<u8>> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (message_ptr, message_len) = into_raw_parts(message);
    let (context_ptr, context_len) = into_raw_parts(context);

    let mut encrypted_message = Vec::new();
    let mut encrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_encrypt_context_imprint(
            master_key_ptr,
            master_key_len,
            message_ptr,
            message_len,
            context_ptr,
            context_len,
            ptr::null_mut(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    encrypted_message.reserve(encrypted_message_len);

    unsafe {
        let status = themis_secure_cell_encrypt_context_imprint(
            master_key_ptr,
            master_key_len,
            message_ptr,
            message_len,
            context_ptr,
            context_len,
            encrypted_message.as_mut_ptr(),
            &mut encrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(encrypted_message_len <= encrypted_message.capacity());
        encrypted_message.set_len(encrypted_message_len);
    }

    Ok(encrypted_message)
}

/// Decrypts `message` with `master_key` and expected `context`, but do not verify data.
fn decrypt_context_imprint(master_key: &[u8], message: &[u8], context: &[u8]) -> Result<Vec<u8>> {
    let (master_key_ptr, master_key_len) = into_raw_parts(master_key);
    let (message_ptr, message_len) = into_raw_parts(message);
    let (context_ptr, context_len) = into_raw_parts(context);

    let mut decrypted_message = Vec::new();
    let mut decrypted_message_len = 0;

    unsafe {
        let status = themis_secure_cell_decrypt_context_imprint(
            master_key_ptr,
            master_key_len,
            message_ptr,
            message_len,
            context_ptr,
            context_len,
            ptr::null_mut(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::BufferTooSmall {
            return Err(error);
        }
    }

    decrypted_message.reserve(decrypted_message_len);

    unsafe {
        let status = themis_secure_cell_decrypt_context_imprint(
            master_key_ptr,
            master_key_len,
            message_ptr,
            message_len,
            context_ptr,
            context_len,
            decrypted_message.as_mut_ptr(),
            &mut decrypted_message_len,
        );
        let error = Error::from_themis_status(status);
        if error.kind() != ErrorKind::Success {
            return Err(error);
        }
        debug_assert!(decrypted_message_len <= decrypted_message.capacity());
        decrypted_message.set_len(decrypted_message_len);
    }

    Ok(decrypted_message)
}
