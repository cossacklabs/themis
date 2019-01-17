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
//! let cell = SecureCell::with_key_and_context(b"seekryt", &[1, 42]).seal();
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
/// This is modeless, basic cell. First you create a `SecureCell` object with a key and an optional
/// context then you select the desired operation mode and your Secure Cell is ready to go.
pub struct SecureCell {
    master_key: KeyBytes,
    user_context: KeyBytes,
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
    /// by one of the [`keygen`] functions (though you definitely can use those keys as well):
    ///
    /// ```
    /// use themis::secure_cell::SecureCell;
    ///
    /// SecureCell::with_key(b"byte string");
    /// SecureCell::with_key(&[1, 2, 3, 4, 5]);
    /// SecureCell::with_key(vec![6, 7, 8, 9]);
    /// SecureCell::with_key(format!("owned string"));
    /// ```
    ///
    /// This method is equivalent to [`with_key_and_context`] called with an empty context (`&[]`):
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell1 = SecureCell::with_key(b"key").seal();
    /// let cell2 = SecureCell::with_key_and_context(b"key", &[]).seal();
    ///
    /// let encrypted = cell1.encrypt(b"some data")?;
    /// let decrypted = cell2.decrypt(&encrypted)?;
    /// assert_eq!(decrypted, b"some data");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// [`keygen`]: ../keygen/index.html
    /// [`with_key_and_context`]: #method.with_key_and_context
    pub fn with_key<K: AsRef<[u8]>>(master_key: K) -> Self {
        Self {
            master_key: KeyBytes::copy_slice(master_key.as_ref()),
            user_context: KeyBytes::empty(),
        }
    }

    /// Constructs a new cell secured by a master key and arbitrary “context information”.
    ///
    /// User context will be included into encrypted data in a manner specific to the operation
    /// mode. See [module-level documentation][secure_cell] for details. You will need to provide
    /// this context again in order to extract the original data from the cell.
    ///
    /// # Examples
    ///
    /// As with the key, the context information can be anything convertible into a byte slice;
    ///
    /// ```
    /// use themis::secure_cell::SecureCell;
    ///
    /// SecureCell::with_key_and_context(&[], b"byte string");
    /// SecureCell::with_key_and_context(&[], &[1, 2, 3, 4, 5]);
    /// SecureCell::with_key_and_context(&[], vec![6, 7, 8, 9]);
    /// SecureCell::with_key_and_context(&[], format!("owned string"));
    /// ```
    ///
    /// [secure_cell]: ../secure_cell/index.html
    pub fn with_key_and_context<K, C>(master_key: K, user_context: C) -> Self
    where
        K: AsRef<[u8]>,
        C: AsRef<[u8]>,
    {
        Self {
            master_key: KeyBytes::copy_slice(master_key.as_ref()),
            user_context: KeyBytes::copy_slice(user_context.as_ref()),
        }
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
/// let cell = SecureCell::with_key(b"password").seal();
///
/// let input = b"test input";
/// let output = cell.encrypt(input)?;
///
/// assert!(output.len() > input.len());
/// # Ok(())
/// # }
/// ```
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
    /// let cell = SecureCell::with_key(b"password").seal();
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
    /// # let cell = SecureCell::with_key(b"password").seal();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// ```
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        encrypt_seal(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
            message.as_ref(),
        )
    }

    /// Extracts the original message from a sealed cell.
    ///
    /// # Examples
    ///
    /// If you know the master key and the context then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password").seal();
    ///
    /// let encrypted = cell.encrypt(b"byte string")?;
    /// let decrypted = cell.decrypt(&encrypted)?;
    /// assert_eq!(decrypted, b"byte string");
    /// # Ok(())
    /// # }
    /// ```
    ///
    /// However, if the key or the context are invalid then decryption fails:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").seal();
    /// # let encrypted = cell.encrypt(b"byte string").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123").seal();
    /// let the_other_cell = SecureCell::with_key_and_context(b"password", b"context").seal();
    ///
    /// assert!(different_cell.decrypt(&encrypted).is_err());
    /// assert!(the_other_cell.decrypt(&encrypted).is_err());
    /// ```
    ///
    /// Secure Cell in sealing mode checks data integrity and can see if the data was corrupted,
    /// returning an error on decryption attempts:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").seal();
    /// # let encrypted = cell.encrypt(b"byte string").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted = encrypted.clone();
    /// corrupted[20] = !corrupted[20];
    ///
    /// assert!(cell.decrypt(&corrupted).is_err());
    /// ```
    pub fn decrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        decrypt_seal(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
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
/// let cell = SecureCell::with_key(b"password").token_protect();
///
/// let input = b"test input";
/// let (output, token) = cell.encrypt(input)?;
///
/// assert!(output.len() == input.len());
/// # Ok(())
/// # }
/// ```
pub struct SecureCellTokenProtect(SecureCell);

impl SecureCellTokenProtect {
    /// Encrypts the provided message and returns the encrypted container and the authentication
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
    /// let cell = SecureCell::with_key(b"password").token_protect();
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
    /// # let cell = SecureCell::with_key(b"password").token_protect();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// ```
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<(Vec<u8>, Vec<u8>)> {
        encrypt_token_protect(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
            message.as_ref(),
        )
    }

    /// Extracts the original message from encrypted container and validates its authenticity.
    ///
    /// You need to provide both the encrypted container and the authentication token previously
    /// obtained from `encrypt()`. Decryption will fail if any of them is corrupted or invalid.
    ///
    /// # Examples
    ///
    /// If you know the master key, the context, and the token then getting back your data is easy:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_cell::SecureCell;
    ///
    /// let cell = SecureCell::with_key(b"password").token_protect();
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
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").token_protect();
    /// # let (encrypted, token) = cell.encrypt(b"byte string").unwrap();
    /// #
    /// let different_cell = SecureCell::with_key(b"qwerty123").token_protect();
    ///
    /// assert!(different_cell.decrypt(&encrypted, &token).is_err());
    /// ```
    ///
    /// Or by the same Secure Cell for different data:
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// # let cell = SecureCell::with_key(b"password").token_protect();
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
    /// # let cell = SecureCell::with_key(b"password").token_protect();
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
    pub fn decrypt<M: AsRef<[u8]>, T: AsRef<[u8]>>(&self, message: M, token: T) -> Result<Vec<u8>> {
        decrypt_token_protect(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
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
/// let cell = SecureCell::with_key_and_context(b"password", b"context").context_imprint();
///
/// let input = b"test input";
/// let output = cell.encrypt(input)?;
///
/// assert!(output.len() == input.len());
/// # Ok(())
/// # }
/// ```
///
/// Note that in context imprint mode you *must* provide non-empty context. Also keep in mind that
/// Secure Cell cannot verify integrity and correctness of the decrypted data so you have to have
/// some other means in place to validate the output.
pub struct SecureCellContextImprint(SecureCell);

// TODO: maybe panic if a SecureCell with an empty context is switched into context imprint mode

impl SecureCellContextImprint {
    /// Encrypts the provided message and returns the encrypted data.
    ///
    /// The resulting message has the same length as the input data and does not contain
    /// an authentication token. Secure Cell cannot ensure correctness of later decryption
    /// if the message gets corrupted or a different master key is used.
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
    /// let cell = SecureCell::with_key_and_context(b"password", b"context").context_imprint();
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
    /// # let cell = SecureCell::with_key_and_context(b"password", b"context").context_imprint();
    /// #
    /// assert!(cell.encrypt(&[]).is_err());
    /// ```
    ///
    /// Also, you cannot use empty context with context imprint mode:
    ///
    /// ```
    /// # use themis::secure_cell::SecureCell;
    /// #
    /// let cell = SecureCell::with_key(b"password").context_imprint();
    ///
    /// assert!(cell.encrypt(b"otherwise fine message").is_err());
    /// ```
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        encrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            self.0.user_context.as_bytes(),
        )
    }

    /// Decrypts the encrypted data and returns the resulting message.
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
    /// let cell = SecureCell::with_key_and_context(b"password", b"context").context_imprint();
    ///
    /// let encrypted = cell.encrypt(b"byte string")?;
    /// let decrypted = cell.decrypt(&encrypted)?;
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
    /// let cell1 = SecureCell::with_key_and_context(b"XXX", b"---").context_imprint();
    /// let cell2 = SecureCell::with_key_and_context(b"OOO", b"|||").context_imprint();
    ///
    /// let encrypted = cell1.encrypt(b"byte string")?;
    /// let decrypted = cell2.decrypt(&encrypted)?;
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
    /// # let cell = SecureCell::with_key_and_context(b"password", b"context").context_imprint();
    /// # let encrypted = cell.encrypt(b"byte string").unwrap();
    /// #
    /// // Let's flip some bits somewhere.
    /// let mut corrupted_data = encrypted.clone();
    /// corrupted_data[10] = !corrupted_data[10];
    ///
    /// let result = cell.decrypt(&corrupted_data);
    /// assert_ne!(result.expect("no verification"), b"byte string");
    /// ```
    pub fn decrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        decrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            self.0.user_context.as_bytes(),
        )
    }
}

/// Encrypts `message` with `master_key` including optional `context`.
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
