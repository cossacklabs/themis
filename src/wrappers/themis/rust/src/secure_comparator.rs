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

//! Secure Comparator service.
//!
//! **Secure Comparator** is an implementation of _Zero-Knowledge Proof_-based protocol,
//! built around OTR SMP implementation.

use std::os::raw::c_void;
use std::ptr;

use bindings::{
    secure_comparator_append_secret, secure_comparator_begin_compare, secure_comparator_create,
    secure_comparator_destroy, secure_comparator_get_result, secure_comparator_proceed_compare,
    secure_comparator_t,
};
use error::{Error, ErrorKind, Result};
use utils::into_raw_parts;

/// Secure Comparison context.
pub struct SecureComparator {
    comp_ctx: *mut secure_comparator_t,
}

impl SecureComparator {
    /// Prepares for a new comparison.
    ///
    /// # Panics
    ///
    /// May panic on internal unrecoverable errors (like memory allocation).
    pub fn new() -> Self {
        match SecureComparator::try_new() {
            Ok(comparator) => comparator,
            Err(e) => panic!("secure_comparator_create() failed: {}", e),
        }
    }

    /// Prepares for a new comparison.
    fn try_new() -> Result<Self> {
        let comp_ctx = unsafe { secure_comparator_create() };

        if comp_ctx.is_null() {
            // This function is most likely to fail on memory allocation. Some internal errors
            // in the crypto library are also possible, but unlikely. We have no way to find out.
            return Err(Error::with_kind(ErrorKind::NoMemory));
        }

        Ok(Self { comp_ctx })
    }

    /// Collects the data to be compared.
    ///
    /// Note that there is no way to remove data. If even a single byte is mismatched by the peers
    /// then the comparison will always return `false`. In this case you will need to recreate
    /// a `SecureComparator` to make a new comparison.
    ///
    /// You can use this method between completed comparisons, but not when you're in the middle
    /// of one. That is, [`append_secret`] is safe call either before [`begin_compare`]
    /// or after [`get_result`]. Otherwise it will fail and return an error.
    ///
    /// [`append_secret`]: struct.SecureComparator.html#method.append_secret
    /// [`begin_compare`]: struct.SecureComparator.html#method.begin_compare
    /// [`get_result`]: struct.SecureComparator.html#method.get_result
    pub fn append_secret<S: AsRef<[u8]>>(&mut self, secret: S) -> Result<()> {
        let (secret_ptr, secret_len) = into_raw_parts(secret.as_ref());

        unsafe {
            let status = secure_comparator_append_secret(
                self.comp_ctx,
                secret_ptr as *const c_void,
                secret_len,
            );
            let error = Error::from_compare_status(status);
            if error.kind() != ErrorKind::Success {
                return Err(error);
            }
        }

        Ok(())
    }

    /// Starts comparison on the client returning the first message.
    ///
    /// This method should be called by the client which initiates the comparison. Make sure you
    /// have appended all the data you need before you call this method.
    ///
    /// The resulting message should be transferred to the remote peer and passed to the
    /// [`proceed_compare`] of its `SecureComparator`. The remote peer should have also appended
    /// all the data by this point.
    ///
    /// [`proceed_compare`]: struct.SecureComparator.html#method.proceed_compare
    pub fn begin_compare(&mut self) -> Result<Vec<u8>> {
        let mut compare_data = Vec::new();
        let mut compare_data_len = 0;

        unsafe {
            let status = secure_comparator_begin_compare(
                self.comp_ctx,
                ptr::null_mut(),
                &mut compare_data_len,
            );
            let error = Error::from_compare_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        compare_data.reserve(compare_data_len);

        unsafe {
            let status = secure_comparator_begin_compare(
                self.comp_ctx,
                compare_data.as_mut_ptr() as *mut c_void,
                &mut compare_data_len,
            );
            let error = Error::from_compare_status(status);
            if error.kind() != ErrorKind::CompareSendOutputToPeer {
                return Err(error);
            }
            debug_assert!(compare_data_len <= compare_data.capacity());
            compare_data.set_len(compare_data_len);
        }

        Ok(compare_data)
    }

    /// Continues comparison process with given message.
    ///
    /// This method should be called by the responding server with a message received from the
    /// client. It returns another message which should be passed back to the client and put
    /// into its [`proceed_compare`] method (that is, this method again). The client then should
    /// do the same. The process repeats at both sides until [`is_complete`] signals that the
    /// comparison is complete.
    ///
    /// Both peers should have appended all the compared data before using this method, and no
    /// additional data should be appended while the comparison is underway.
    ///
    /// [`proceed_compare`]: struct.SecureComparator.html#method.proceed_compare
    /// [`is_complete`]: struct.SecureComparator.html#method.is_complete
    pub fn proceed_compare<D: AsRef<[u8]>>(&mut self, peer_data: D) -> Result<Vec<u8>> {
        let (peer_compare_data_ptr, peer_compare_data_len) = into_raw_parts(peer_data.as_ref());

        let mut compare_data = Vec::new();
        let mut compare_data_len = 0;

        unsafe {
            let status = secure_comparator_proceed_compare(
                self.comp_ctx,
                peer_compare_data_ptr as *const c_void,
                peer_compare_data_len,
                ptr::null_mut(),
                &mut compare_data_len,
            );
            let error = Error::from_compare_status(status);
            if error.kind() != ErrorKind::BufferTooSmall {
                return Err(error);
            }
        }

        compare_data.reserve(compare_data_len);

        unsafe {
            let status = secure_comparator_proceed_compare(
                self.comp_ctx,
                peer_compare_data_ptr as *const c_void,
                peer_compare_data_len,
                compare_data.as_mut_ptr() as *mut c_void,
                &mut compare_data_len,
            );
            let error = Error::from_compare_status(status);
            match error.kind() {
                ErrorKind::CompareSendOutputToPeer => {}
                // TODO: signal that this does not need to be sent
                ErrorKind::Success => {}
                _ => {
                    return Err(error);
                }
            }
            debug_assert!(compare_data_len <= compare_data.capacity());
            compare_data.set_len(compare_data_len);
        }

        Ok(compare_data)
    }

    /// Returns the result of comparison.
    ///
    /// Let it be a surprise: `true` if data has been found equal on both peers, `false` otherwise.
    /// Or an error if you call this method too early.
    pub fn get_result(&self) -> Result<bool> {
        let status = unsafe { secure_comparator_get_result(self.comp_ctx) };
        let error = Error::from_match_status(status);
        match error.kind() {
            ErrorKind::CompareMatch => Ok(true),
            ErrorKind::CompareNoMatch => Ok(false),
            _ => Err(error),
        }
    }

    /// Checks if this comparison is complete.
    ///
    /// Comparison that failed irrecoverably due to an error is also considered complete.
    pub fn is_complete(&self) -> bool {
        match self.get_result() {
            Err(ref e) if e.kind() == ErrorKind::CompareNotReady => false,
            _ => true,
        }
    }
}

impl Default for SecureComparator {
    fn default() -> Self {
        SecureComparator::new()
    }
}

#[doc(hidden)]
impl Drop for SecureComparator {
    fn drop(&mut self) {
        unsafe {
            let status = secure_comparator_destroy(self.comp_ctx);
            let error = Error::from_themis_status(status);
            if (cfg!(debug) || cfg!(test)) && error.kind() != ErrorKind::Success {
                panic!("secure_comparator_destroy() failed: {}", error);
            }
        }
    }
}
