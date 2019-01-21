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

//! Secure Comparator protocol.
//!
//! **Secure Comparator** is an interactive protocol for two parties that compares whether they
//! share the same secret or not. It is built around a [Zero Knowledge Proof][ZKP]-based protocol
//! ([Socialist Millionaire’s Protocol][SMP]), with a number of [security enhancements][paper].
//!
//! Secure Comparator is transport-agnostic and only requires the user(s) to pass messages
//! in a certain sequence. The protocol itself is ingrained into the functions and requires
//! minimal integration efforts from the developer.
//!
//! [ZKP]: https://www.cossacklabs.com/zero-knowledge-protocols-without-magic.html
//! [SMP]: https://en.wikipedia.org/wiki/Socialist_millionaires
//! [paper]: https://www.cossacklabs.com/files/secure-comparator-paper-rev12.pdf
//!
//! # Examples
//!
//! Secure Comparator has two parties — called the client and the server — the only difference
//! between them is that the client is the one who initiates the comparison.
//!
//! Before initiating the protocol both parties should [append their secrets] to be compared.
//! This can be done incrementally so even multi-gigabyte data sets can be compared with ease.
//!
//! [append their secrets]: struct.SecureComparator.html#method.append_secret
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! use themis::secure_comparator::SecureComparator;
//!
//! let mut comparison = SecureComparator::new();
//!
//! comparison.append_secret(b"999-04-1234")?;
//! # Ok(())
//! # }
//! ```
//!
//! After that the client [initiates the comparison] and runs a loop like this:
//!
//! [initiates the comparison]: struct.SecureComparator.html#method.begin_compare
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! # use std::cell::RefCell;
//! #
//! # use themis::secure_comparator::SecureComparator;
//! #
//! # let mut comparison = SecureComparator::new();
//! # let mut other_peer = SecureComparator::new();
//! #
//! # comparison.append_secret(b"999-04-1234").expect("append client");
//! # other_peer.append_secret(b"999-04-1234").expect("append server");
//! #
//! # let peer_data = RefCell::new(None);
//! # let mut send = |data: &[u8]| {
//! #     let reply = other_peer.proceed_compare(data).expect("server comparison");
//! #     peer_data.replace(Some(reply));
//! # };
//! # let mut receive = || {
//! #     peer_data.borrow_mut().take().expect("reply data")
//! # };
//! #
//! let mut request = comparison.begin_compare()?;
//!
//! while !comparison.is_complete() {
//!     send(&request);         // This function should send the `request` to the server.
//!     let reply = receive();  // This function should receive a `reply` from the server.
//!
//!     request = comparison.proceed_compare(&reply)?;
//! }
//!
//! if !comparison.get_result()? {
//!     unimplemented!("handle failed comparison here");
//! }
//! # Ok(())
//! # }
//! ```
//!
//! While the server does almost the same:
//!
//! ```
//! # fn main() -> Result<(), themis::Error> {
//! # use std::cell::RefCell;
//! #
//! # use themis::secure_comparator::SecureComparator;
//! #
//! # let mut comparison = SecureComparator::new();
//! # let mut other_peer = SecureComparator::new();
//! #
//! # comparison.append_secret(b"999-04-1234").expect("append server");
//! # other_peer.append_secret(b"999-04-1234").expect("append client");
//! # let request = other_peer.begin_compare().expect("begin client");
//! #
//! # let peer_data = RefCell::new(Some(request));
//! # let mut send = |data: &[u8]| {
//! #     let reply = other_peer.proceed_compare(data).expect("server comparison");
//! #     peer_data.replace(Some(reply));
//! # };
//! # let mut receive = || {
//! #     peer_data.borrow_mut().take().expect("reply data")
//! # };
//! #
//! while !comparison.is_complete() {
//!     // This function should receive a `request` from the client.
//!     let request = receive();
//!
//!     let reply = comparison.proceed_compare(&request)?;
//!
//!     send(&reply);   // This function should send the `reply` to the client.
//! }
//!
//! if !comparison.get_result()? {
//!     unimplemented!("handle failed comparison here");
//! }
//! # Ok(())
//! # }
//! ```
//!
//! Both the server and the client use [`get_result`] to get the comparison result
//! after it [`is_complete`]:
//!
//! [`get_result`]: struct.SecureComparator.html#method.get_result
//! [`is_complete`]: struct.SecureComparator.html#method.is_complete

use std::os::raw::c_void;
use std::ptr;

use bindings::{
    secure_comparator_append_secret, secure_comparator_begin_compare, secure_comparator_create,
    secure_comparator_destroy, secure_comparator_get_result, secure_comparator_proceed_compare,
    secure_comparator_t,
};

use crate::error::{Error, ErrorKind, Result};
use crate::utils::into_raw_parts;

/// Secure Comparison context.
///
/// Please see [module-level documentation][secure_comparator] for examples.
///
/// [secure_comparator]: index.html
pub struct SecureComparator {
    comp_ctx: *mut secure_comparator_t,
}

impl SecureComparator {
    /// Prepares a new comparison.
    ///
    /// # Panics
    ///
    /// May panic on internal unrecoverable errors (e.g., out-of-memory).
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
    /// You can use this method only before the comparison has been started. That is,
    /// [`append_secret`] is safe call only before [`begin_compare`] or [`proceed_compare`].
    /// It will fail with an error if you try to append more data when you’re in the middle of
    /// a comparison or after it has been completed.
    ///
    /// [`append_secret`]: struct.SecureComparator.html#method.append_secret
    /// [`begin_compare`]: struct.SecureComparator.html#method.begin_compare
    /// [`proceed_compare`]: struct.SecureComparator.html#method.proceed_compare
    ///
    /// # Examples
    ///
    /// You can pass in anything convertible into a byte slice: a byte slice or an array,
    /// a `Vec<u8>`, or a `String`.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_comparator::SecureComparator;
    ///
    /// let mut comparison = SecureComparator::new();
    ///
    /// comparison.append_secret(b"byte string")?;
    /// comparison.append_secret(&[1, 2, 3, 4, 5])?;
    /// comparison.append_secret(vec![6, 7, 8, 9])?;
    /// comparison.append_secret(format!("owned string"))?;
    /// # Ok(())
    /// # }
    /// ```
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
    ///
    /// # Examples
    ///
    /// Please see [module-level documentation][secure_comparator] for examples.
    ///
    /// [secure_comparator]: index.html
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
    /// additional data may be appended while the comparison is underway.
    ///
    /// [`proceed_compare`]: struct.SecureComparator.html#method.proceed_compare
    /// [`is_complete`]: struct.SecureComparator.html#method.is_complete
    ///
    /// # Examples
    ///
    /// Please see [module-level documentation][secure_comparator] for examples.
    ///
    /// [secure_comparator]: index.html
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
    /// Or an error if you call this method too early, or if a real error has happened during the
    /// comparison.
    ///
    /// # Examples
    ///
    /// You should call this method only after the comparison is complete.
    ///
    /// ```
    /// # fn main() -> Result<(), themis::Error> {
    /// use themis::secure_comparator::SecureComparator;
    /// #
    /// # use std::cell::RefCell;
    ///
    /// let mut comparison = SecureComparator::new();
    /// # let mut other_peer = SecureComparator::new();
    ///
    /// comparison.append_secret(b"999-04-1234")?;
    /// # other_peer.append_secret(b"999-04-1234")?;
    ///
    /// assert!(comparison.get_result().is_err());
    ///
    /// // Perform comparison
    /// #
    /// # let mut request = comparison.begin_compare()?;
    /// #
    /// while !comparison.is_complete() {
    ///     // ...
    /// #   let reply = other_peer.proceed_compare(&request)?;
    /// #   request = comparison.proceed_compare(&reply)?;
    /// }
    ///
    /// assert!(comparison.get_result().is_ok());
    /// # Ok(())
    /// # }
    /// ```
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
    ///
    /// # Examples
    ///
    /// Typically you would use this method to terminate the comparison loop. Please see
    /// [module-level documentation][secure_comparator] for examples.
    ///
    /// [secure_comparator]: index.html
    ///
    /// It is safe to call this method at any point, even if the comparison has not been initiated
    /// yet (in which case it is obviously not complete):
    ///
    /// ```
    /// use themis::secure_comparator::SecureComparator;
    ///
    /// let mut comparison = SecureComparator::new();
    ///
    /// assert!(!comparison.is_complete());
    /// ```
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
