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

//! Secure Cell service.
//!
//! **Secure Сell** is a high-level cryptographic service aimed at protecting arbitrary data
//! stored in various types of storage (e.g., databases, filesystem files, document archives,
//! cloud storage, etc.)

use std::ptr;

use bindings::{
    themis_secure_cell_decrypt_context_imprint, themis_secure_cell_decrypt_seal,
    themis_secure_cell_decrypt_token_protect, themis_secure_cell_encrypt_context_imprint,
    themis_secure_cell_encrypt_seal, themis_secure_cell_encrypt_token_protect,
};
use error::{Error, ErrorKind, Result};
use keys::KeyBytes;
use utils::into_raw_parts;

/// Basic Secure Cell.
///
/// This is modeless, basic cell. Specific operation mode must be selected by one of the calls:
///
/// * `seal()`
/// * `token_protect()`
/// * `context_imprint()`
pub struct SecureCell {
    master_key: KeyBytes,
    user_context: KeyBytes,
}

impl SecureCell {
    /// Constructs a new cell secured by a master key.
    pub fn with_key<K: AsRef<[u8]>>(master_key: K) -> Self {
        Self {
            master_key: KeyBytes::copy_slice(master_key.as_ref()),
            user_context: KeyBytes::empty(),
        }
    }

    /// Constructs a new cell secured by a master key and arbitrary user data.
    ///
    /// User context will be included into encrypted data in a manner specific to the operation
    /// mode. See module-level documentation for details. You will need to provide this context
    /// again in order to extract the original data from the cell.
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

/// Secure Cell in a _sealing_ operation mode.
pub struct SecureCellSeal(SecureCell);

impl SecureCellSeal {
    /// Encrypts and puts the provided message into a sealed cell.
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        encrypt_seal(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
            message.as_ref(),
        )
    }

    /// Extracts the original message from a sealed cell.
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

/// Secure Cell in a _token protect_ operation mode.
pub struct SecureCellTokenProtect(SecureCell);

impl SecureCellTokenProtect {
    /// Encrypts the provided message and returns the ciphertext with authentication token.
    ///
    /// The ciphertext and authentication token could be stored or transmitted separately.
    /// You will need to provide both later for successful decryption.
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<(Vec<u8>, Vec<u8>)> {
        encrypt_token_protect(
            self.0.master_key.as_bytes(),
            self.0.user_context.as_bytes(),
            message.as_ref(),
        )
    }

    /// Decrypts the ciphertext then validates and returns the original message.
    ///
    /// You need to provide both the ciphertext and the authentication token previously obtained
    /// from `encrypt()`. Decryption will fail if any of them is corrupted or invalid.
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

/// Secure Cell in a _context imprint_ operation mode.
pub struct SecureCellContextImprint(SecureCell);

impl SecureCellContextImprint {
    /// Encrypts the provided message and returns the ciphertext.
    ///
    /// The resulting message has the same length as the input data and does not contain
    /// an authentication token. There is no way to ensure correctness of later decryption
    /// if the message gets corrupted or misplaced.
    pub fn encrypt<M: AsRef<[u8]>>(&self, message: M) -> Result<Vec<u8>> {
        encrypt_context_imprint(
            self.0.master_key.as_bytes(),
            message.as_ref(),
            self.0.user_context.as_bytes(),
        )
    }

    /// Decrypts the ciphertext and returns the original message.
    ///
    /// Note that in context imprint mode the messages do not include any authentication token
    /// for validation, thus the returned message might not be the original one even if has been
    /// decrypted successfully.
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
