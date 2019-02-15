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
//!   - _master key:_ a password
//!   - _context:_ and an optional “context information”
//!
//! Secure Cell will produce:
//!
//!   - _cell:_ the encrypted data
//!   - _authentication tag:_ some authentication data
//!
//! The purpose of the optional “context information” (e.g., a database row number or file name)
//! is to establish a secure association between this context and the protected data. In short,
//! even when the password is known, if the context is incorrect then decryption will fail.
//!
//! The purpose of the authentication data is to validate that given a correct password
//! (and context), the decrypted data is indeed the same as the original source data.
//!
//! The authentication data must be stored somewhere. The most convenient way is to simply
//! append it to the encrypted data, but this is not always possible due to the storage
//! architecture of your application. Secure Cell offers variants that address this issue
//! in different ways.
//!
//! By default, Secure Cell uses AES-256 for encryption. Authentication data is 16 bytes long.
//!
//! Secure Cell supports 3 operation modes:
//!
//!   - **[Sealing mode]:** the mode that is the most secure and easy to use.
//!     Your best choice most of the time.
//!
//!   - **[Token protect mode]:** the mode just as secure, but a bit harder
//!     to use. This is your choice if you need to keep authentication data
//!     separately.
//!
//!   - **[Context imprint mode]:** length-preserving version of Secure Cell
//!     with no additional data stored. Should be used carefully.
//!
//! The operation mode is selected via an appropriate method of a basic [`SecureCell`] object.
//! [Here you can learn more](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem)
//! about the underlying considerations, limitations, and features of each mode.
//!
//! [`SecureCell`]: struct.SecureCell.html
//! [Sealing mode]: struct.SecureCellSeal.html
//! [Token protect mode]: struct.SecureCellTokenProtect.html
//! [Context imprint mode]: struct.SecureCellContextImprint.html
//!
//! # Examples
//!
//! Here is how you use Secure Cell to seal away your data:
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! use themis::secure_cell::SecureCell;
//!
//! let cell = SecureCell::with_key(b"seekryt")?.seal();
//!
//! let encrypted = cell.encrypt(b"source data")?;
//! let decrypted = cell.decrypt(&encrypted)?;
//! assert_eq!(decrypted, b"source data");
//! # Ok(())
//! # }
//! ```
//!
//! You can find more examples for each operation mode in their respective documentation.

use std::ptr;

use bindings::{
    themis_secure_cell_decrypt_context_imprint, themis_secure_cell_decrypt_seal,
    themis_secure_cell_decrypt_token_protect, themis_secure_cell_encrypt_context_imprint,
    themis_secure_cell_encrypt_seal, themis_secure_cell_encrypt_token_protect,
};

use crate::error::{Error, ErrorKind, Result};
use crate::keys::KeyBytes;
use crate::utils::into_raw_parts;

/// Basic Secure Cell.
///
/// This is modeless, basic cell. First you provide the master key to a new `SecureCell` object
/// then you select the desired operation mode and your Secure Cell is ready to go.
#[derive(Debug)]
pub struct SecureCell {
    master_key: KeyBytes,
}

impl SecureCell {
    /// Constructs a new cell secured by a master key.
    ///
    /// # Examples
    ///
    /// A master key may be provided as anything convertible to a byte slice: a byte slice or an
    /// array, a `Vec<u8>`, or a `String`.
    ///
    /// Note that it is actually _a password_. It is not required to be an actual key generated
    /// by one of the [`keygen`] functions (though you definitely can use those keys as well).
    /// The master key cannot be empty.
    ///
    /// ```
    /// use themis::secure_cell::SecureCell;
    ///
    /// assert!(SecureCell::with_key(b"byte string").is_ok());
    /// assert!(SecureCell::with_key(&[1, 2, 3, 4, 5]).is_ok());
    /// assert!(SecureCell::with_key(vec![6, 7, 8, 9]).is_ok());
    /// assert!(SecureCell::with_key(format!("owned string")).is_ok());
    ///
    /// assert!(SecureCell::with_key(b"").is_err());
    /// ```
    ///
    /// [`keygen`]: ../keygen/index.html
    pub fn with_key(master_key: impl AsRef<[u8]>) -> Result<Self> {
        Ok(Self {
            master_key: KeyBytes::copy_slice(master_key.as_ref())?,
        })
    }

    /// Switches this Secure Cell to the _sealing_ operation mode.
    pub fn seal(self) -> SecureCellSeal {
        SecureCellSeal(self)
    }

    /// Switches this Secure Cell to the _token protect_ operation mode.
    pub fn token_protect(self) -> SecureCellTokenProtect {
        SecureCellTokenProtect(self)
    }

    /// Switches this Secure Cell to the _context imprint_ operation mode.
    pub fn context_imprint(self) -> SecureCellContextImprint {
        SecureCellContextImprint(self)
    }
}

/// Secure Cell in _sealing_ operation mode.
///
/// In this mode the input data is mixed with the provided context and encrypted, then the
/// authentication tag is appended to the data, resulting in a single encrypted and authenticated
/// container. Note that the resulting sealed cell takes more space than the input data:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_cell::SecureCell;
///
/// let cell = SecureCell::with_key(b"password")?.seal();
///
/// let input = b"test input";
/// let output = cell.encrypt(input)?;
///
/// assert!(output.len() > input.len());
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureCellSeal(SecureCell);

impl SecureCellSeal {
    /// Encrypts and puts the provided message into a sealed cell.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message: a byte slice or an array,
    /// a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.seal();
    ///
    /// cell.encrypt(b"byte string")?;
    /// cell.encrypt(&[1, 2, 3, 4, 5])?;
    /// cell.encrypt(vec![6, 7, 8, 9])?;
    /// cell.encrypt(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message must not be empty:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.encrypt_with_context(message, &[])
    }

    /// Encrypts and puts the provided message together with the context into a sealed cell.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message or a context: a byte slice
    /// or an array, a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.seal();
    ///
    /// cell.encrypt_with_context(b"byte string", format!("owned string"))?;
    /// cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10])?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context may be empty (in which case this call is equivalent to [`encrypt`]).
    /// However, the message must not be empty.
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", &[]).is_ok());
    /// assert!(cell.encrypt_with_context(&[], b"context").is_err());
    /// ```
    ///
    /// [`encrypt`]: struct.SecureCellSeal.html#method.encrypt
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        encrypt_seal(
            self.0.master_key.as_bytes(),
            user_context.as_ref(),
            message.as_ref(),
        )
    }

    /// Extracts the original message from a sealed cell.
    ///
    /// # Examples
    ///
    /// If you know the master key then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.seal();
    ///
    /// let encrypted = cell.encrypt(b"byte string")?;
    /// let decrypted = cell.decrypt(&encrypted)?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, if the key is invalid then decryption fails:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// # let encrypted = cell.encrypt(b"byte string").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123")?.seal();
    ///
    /// assert!(different_cell.decrypt(&encrypted).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in sealing mode checks data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// # let encrypted = cell.encrypt(b"byte string").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt(&corrupted).is_err());
    /// ```
    pub fn decrypt(&self, message: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.decrypt_with_context(message, &[])
    }

    /// Extracts the original message from a sealed cell given the context.
    ///
    /// # Examples
    ///
    /// If you know the master key and the context then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.seal();
    ///
    /// let encrypted = cell.encrypt_with_context(b"byte string", b"context")?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, b"context")?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, if the key or the context are invalid then decryption fails:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// # let encrypted = cell.encrypt_with_context(b"byte string", b"context").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123")?.seal();
    ///
    /// assert!(different_cell.decrypt_with_context(&encrypted, b"context").is_err());
    /// assert!(cell.decrypt_with_context(&encrypted, b"different context").is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in sealing mode checks data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().seal();
    /// # let encrypted = cell.encrypt_with_context(b"byte string", b"context").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt_with_context(&corrupted, b"context").is_err());
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_seal(
            self.0.master_key.as_bytes(),
            user_context.as_ref(),
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

    encrypted_message.reserve(encrypted_message_len as usize);

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
        encrypted_message.set_len(encrypted_message_len as usize);
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

    decrypted_message.reserve(decrypted_message_len as usize);

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
        decrypted_message.set_len(decrypted_message_len as usize);
    }

    Ok(decrypted_message)
}

/// Secure Cell in _token protect_ operation mode.
///
/// In this mode the input data is mixed with the provided context and encrypted, then the
/// authentication token is computed and returned separately, along with the encrypted container.
/// You will have to provide the authentication token later to decrypt the data, but it can be
/// stored or transmitted separately. The encrypted data has the same length as the original input.
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_cell::SecureCell;
///
/// let cell = SecureCell::with_key(b"password")?.token_protect();
///
/// let input = b"test input";
/// let (output, token) = cell.encrypt(input)?;
///
/// assert!(output.len() == input.len());
/// # Ok(())
/// # }
/// ```
#[derive(Debug)]
pub struct SecureCellTokenProtect(SecureCell);

impl SecureCellTokenProtect {
    /// Encrypts the provided message and returns the encrypted container with the authentication
    /// token (in that order).
    ///
    /// The results can be stored or transmitted separately. You will need to provide both later
    /// for successful decryption.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message: a byte slice or an array,
    /// a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.token_protect();
    ///
    /// cell.encrypt(b"byte string")?;
    /// cell.encrypt(&[1, 2, 3, 4, 5])?;
    /// cell.encrypt(vec![6, 7, 8, 9])?;
    /// cell.encrypt(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message must not be empty:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// ```
    pub fn encrypt(&self, message: impl AsRef<[u8]>) -> Result<(Vec<u8>, Vec<u8>)> {
        self.encrypt_with_context(message, &[])
    }

    /// Encrypts the provided message together with the context and returns the encrypted container
    /// with the authentication token (in that order).
    ///
    /// The results can be stored or transmitted separately. You will need to provide all three
    /// parts later for successful decryption.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message or a context: a byte slice
    /// or an array, a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.token_protect();
    ///
    /// cell.encrypt_with_context(b"byte string", format!("owned string"))?;
    /// cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10])?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// The context may be empty (in which case this call is equivalent to [`encrypt`]).
    /// However, the message must not be empty.
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", &[]).is_ok());
    /// assert!(cell.encrypt_with_context(&[], b"context").is_err());
    /// ```
    ///
    /// [`encrypt`]: struct.SecureCellTokenProtect.html#method.encrypt
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<(Vec<u8>, Vec<u8>)> {
        encrypt_token_protect(
            self.0.master_key.as_bytes(),
            user_context.as_ref(),
            message.as_ref(),
        )
    }

    /// Extracts the original message from encrypted container and validates its authenticity.
    ///
    /// You need to provide both the encrypted container and the authentication token previously
    /// obtained from [`encrypt`]. Decryption will fail if any of them is corrupted or invalid.
    ///
    /// [`encrypt`]: struct.SecureCellTokenProtect.html#method.encrypt
    ///
    /// # Examples
    ///
    /// If you know the master key and the token then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.token_protect();
    ///
    /// let (encrypted, token) = cell.encrypt(b"byte string")?;
    /// let decrypted = cell.decrypt(&encrypted, &token)?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, you obviously cannot use tokens produced by Secure Cells with different keys:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// # let (encrypted, token) = cell.encrypt(b"byte string").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123")?.token_protect();
    ///
    /// assert!(different_cell.decrypt(&encrypted, &token).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Or by the same Secure Cell for different data:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// #
    /// let (encrypted,   _) = cell.encrypt(b"byte string")?;
    /// let (_, other_token) = cell.encrypt(b"other data")?;
    ///
    /// assert!(cell.decrypt(&encrypted, &other_token).is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in token protect mode checks data integrity and can see if the data (or the
    /// token) was corrupted, returning an error on decryption attempts:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// # let (encrypted, auth_token) = cell.encrypt(b"byte string").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted_data = encrypted.clone();
    /// let mut corrupted_token = auth_token.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    /// corrupted_token[9] = !corrupted_token[9];
    ///
    /// assert!(cell.decrypt(&corrupted_data, &auth_token).is_err());
    /// assert!(cell.decrypt(&encrypted, &corrupted_token).is_err());
    /// ```
    pub fn decrypt(&self, message: impl AsRef<[u8]>, token: impl AsRef<[u8]>) -> Result<Vec<u8>> {
        self.decrypt_with_context(message, token, &[])
    }

    /// Extracts the original message from encrypted container and validates its authenticity
    /// given the context.
    ///
    /// You need to provide the user context used for encryption as well as the encrypted container
    /// and the authentication token previously obtained from [`encrypt_with_context`]. Decryption
    /// will fail if any of them is corrupted or invalid.
    ///
    /// [`encrypt_with_context`]: struct.SecureCellTokenProtect.html#method.encrypt_with_context
    ///
    /// # Examples
    ///
    /// If you know the master key, the context, and the token then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.token_protect();
    ///
    /// let (encrypted, token) = cell.encrypt_with_context(b"byte string", b"context")?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, &token, b"context")?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, you obviously cannot use tokens produced by Secure Cells with different keys,
    /// tokens for different data, or different contexts:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// # let (encrypted, token) = cell.encrypt_with_context(b"byte string", b"context").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123")?.token_protect();
    ///
    /// assert!(different_cell.decrypt_with_context(&encrypted, &token, b"context").is_err());
    ///
    /// let (_, other_token) = cell.encrypt_with_context(b"other data", b"context")?;
    ///
    /// assert!(cell.decrypt_with_context(&encrypted, &other_token, b"context").is_err());
    /// assert!(cell.decrypt_with_context(&encrypted, &token, b"other context").is_err());
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Secure Cell in token protect mode checks data integrity and can see if the data (or the
    /// token) was corrupted, returning an error on decryption attempts:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().token_protect();
    /// # let (encrypted, auth_token) = cell.encrypt_with_context(b"things", b"context").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted_data = encrypted.clone();
    /// let mut corrupted_token = auth_token.clone();
    /// corrupted_data[4] = !corrupted_data[4];
    /// corrupted_token[9] = !corrupted_token[9];
    ///
    /// assert!(cell.decrypt_with_context(&corrupted_data, &auth_token, b"context").is_err());
    /// assert!(cell.decrypt_with_context(&encrypted, &corrupted_token, b"context").is_err());
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        token: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_token_protect(
            self.0.master_key.as_bytes(),
            user_context.as_ref(),
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

    token.reserve(token_len as usize);
    encrypted_message.reserve(encrypted_message_len as usize);

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
        token.set_len(token_len as usize);
        debug_assert!(encrypted_message_len <= encrypted_message.capacity());
        encrypted_message.set_len(encrypted_message_len as usize);
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

    decrypted_message.reserve(decrypted_message_len as usize);

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
        decrypted_message.set_len(decrypted_message_len as usize);
    }

    Ok(decrypted_message)
}

/// Secure Cell in _context imprint_ operation mode.
///
/// In this mode the input data is mixed with the provided context and encrypted, but there is no
/// authentication token. Use this mode when you have no additional storage available for the
/// authentication data and you absolutely need the output data to have the same length as the
/// original input:
///
/// ```
/// # fn main() -> Result<(), themis::Error> {
/// use themis::secure_cell::SecureCell;
///
/// let cell = SecureCell::with_key(b"password")?.context_imprint();
///
/// let input = b"test input";
/// let output = cell.encrypt_with_context(input, b"context")?;
///
/// assert!(output.len() == input.len());
/// # Ok(())
/// # }
/// ```
///
/// Note that in context imprint mode you *must* provide non-empty context. Also keep in mind that
/// Secure Cell cannot verify integrity and correctness of the decrypted data so you have to have
/// some other means in place to validate the output.
#[derive(Debug)]
pub struct SecureCellContextImprint(SecureCell);

impl SecureCellContextImprint {
    /// Encrypts the provided message, combining it with provided user context, and returns
    /// the encrypted data.
    ///
    /// The resulting message has the same length as the input data and does not contain
    /// an authentication token. Secure Cell cannot ensure correctness of later decryption
    /// if the message gets corrupted or a different master key is used.
    ///
    /// # Examples
    ///
    /// You can use anything convertible into a byte slice as a message or a context: a byte slice
    /// or an array, a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.context_imprint();
    ///
    /// cell.encrypt_with_context(b"byte string", format!("owned string"))?;
    /// cell.encrypt_with_context(&[1, 2, 3, 4, 5], vec![6, 7, 8, 9, 10])?;
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, the message and context must not be empty:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().context_imprint();
    /// #
    /// assert!(cell.encrypt_with_context(b"message", b"context").is_ok());
    /// assert!(cell.encrypt_with_context(b"",        b"context").is_err());
    /// assert!(cell.encrypt_with_context(b"message",        b"").is_err());
    /// ```
    pub fn encrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        encrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            user_context.as_ref(),
        )
    }

    /// Decrypts the encrypted data with provided user context and returns the resulting message.
    ///
    /// Note that in context imprint mode messages do not include any authentication token for
    /// integrity validation, thus the returned message might not be the original one even if
    /// it has been decrypted ‘successfully’.
    ///
    /// # Examples
    ///
    /// If you know the master key and the context then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password")?.context_imprint();
    ///
    /// let encrypted = cell.encrypt_with_context(b"byte string", b"context")?;
    /// let decrypted = cell.decrypt_with_context(&encrypted, b"context")?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, Secure Cell in context imprint mode will not verify that the key or the message
    /// were decrypted correctly:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// let cell1 = SecureCell::with_key(b"XXX")?.context_imprint();
    /// let cell2 = SecureCell::with_key(b"OOO")?.context_imprint();
    ///
    /// let encrypted = cell1.encrypt_with_context(b"byte string", b"context")?;
    /// let decrypted = cell2.decrypt_with_context(&encrypted, b"task-switch")?;
    ///
    /// assert_ne!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// Or whether the message has been corrupted:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").unwrap().context_imprint();
    /// # let encrypted = cell.encrypt_with_context(b"byte string", b"context").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted_data = encrypted.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    ///
    /// let result = cell.decrypt_with_context(&corrupted_data, b"context");
    /// assert_ne!(result.expect("no verification"), b"byte string");
    /// ```
    pub fn decrypt_with_context(
        &self,
        message: impl AsRef<[u8]>,
        user_context: impl AsRef<[u8]>,
    ) -> Result<Vec<u8>> {
        decrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            user_context.as_ref(),
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

    encrypted_message.reserve(encrypted_message_len as usize);

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
        encrypted_message.set_len(encrypted_message_len as usize);
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

    decrypted_message.reserve(decrypted_message_len as usize);

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
        decrypted_message.set_len(decrypted_message_len as usize);
    }

    Ok(decrypted_message)
}
