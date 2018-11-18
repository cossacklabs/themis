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

//! Themis error types.
//!
//! This module wraps Themis error types and provides useful Rust API for them.

use std::{error, fmt, result};

use bindings::{
    THEMIS_BUFFER_TOO_SMALL, THEMIS_DATA_CORRUPT, THEMIS_FAIL, THEMIS_INVALID_PARAMETER,
    THEMIS_INVALID_SIGNATURE, THEMIS_NOT_SUPPORTED, THEMIS_NO_MEMORY, THEMIS_SCOMPARE_MATCH,
    THEMIS_SCOMPARE_NOT_READY, THEMIS_SCOMPARE_NO_MATCH, THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER,
    THEMIS_SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR, THEMIS_SSESSION_KA_NOT_FINISHED,
    THEMIS_SSESSION_SEND_OUTPUT_TO_PEER, THEMIS_SSESSION_TRANSPORT_ERROR, THEMIS_SUCCESS,
};

/// Themis status code.
pub(crate) use bindings::themis_status_t;

/// Result type for most Themis operations.
pub type Result<T> = result::Result<T, Error>;

/// The error type for most Themis operations.
///
/// Errors are usually caused by invalid, malformed or malicious input as well as incorrect usage
/// of the library. However, they may also result from underlying OS errors. See [`ErrorKind`] for
/// details.
///
/// [`ErrorKind`]: enum.ErrorKind.html
#[derive(Debug, Clone, Eq, PartialEq)]
pub struct Error {
    kind: ErrorKind,
}

impl Error {
    /// Constructs a new error of given kind.
    pub(crate) fn with_kind(kind: ErrorKind) -> Error {
        Error { kind }
    }

    /// Converts generic Themis status codes.
    pub(crate) fn from_themis_status(status: themis_status_t) -> Error {
        let kind = match status as u32 {
            THEMIS_SUCCESS => ErrorKind::Success,
            THEMIS_FAIL => ErrorKind::Fail,
            THEMIS_INVALID_PARAMETER => ErrorKind::InvalidParameter,
            THEMIS_NO_MEMORY => ErrorKind::NoMemory,
            THEMIS_BUFFER_TOO_SMALL => ErrorKind::BufferTooSmall,
            THEMIS_DATA_CORRUPT => ErrorKind::DataCorrupt,
            THEMIS_INVALID_SIGNATURE => ErrorKind::InvalidSignature,
            THEMIS_NOT_SUPPORTED => ErrorKind::NotSupported,
            _ => ErrorKind::UnknownError(status),
        };
        Error { kind }
    }

    /// Converts status codes returned by Secure Session.
    pub(crate) fn from_session_status(status: themis_status_t) -> Error {
        let kind = match status as u32 {
            THEMIS_SSESSION_SEND_OUTPUT_TO_PEER => ErrorKind::SessionSendOutputToPeer,
            THEMIS_SSESSION_KA_NOT_FINISHED => ErrorKind::SessionKeyAgreementNotFinished,
            THEMIS_SSESSION_TRANSPORT_ERROR => ErrorKind::SessionTransportError,
            THEMIS_SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR => {
                ErrorKind::SessionGetPublicKeyForIdError
            }
            _ => return Error::from_themis_status(status),
        };
        Error { kind }
    }

    /// Converts status codes returned by Secure Comparator data exchange.
    pub(crate) fn from_compare_status(status: themis_status_t) -> Error {
        let kind = match status as u32 {
            THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER => ErrorKind::CompareSendOutputToPeer,
            _ => return Error::from_themis_status(status),
        };
        Error { kind }
    }

    /// Converts status codes returned by Secure Comparator status query.
    pub(crate) fn from_match_status(status: themis_status_t) -> Error {
        let kind = match status as u32 {
            THEMIS_SCOMPARE_NOT_READY => ErrorKind::CompareNotReady,
            THEMIS_SCOMPARE_MATCH => ErrorKind::CompareMatch,
            THEMIS_SCOMPARE_NO_MATCH => ErrorKind::CompareNoMatch,
            _ => return Error::from_themis_status(status),
        };
        Error { kind }
    }

    /// Returns the corresponding `ErrorKind` for this error.
    pub fn kind(&self) -> ErrorKind {
        self.kind
    }
}

impl error::Error for Error {}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self.kind {
            ErrorKind::UnknownError(status) => write!(f, "unknown error: {}", status),
            ErrorKind::Success => write!(f, "success"),

            ErrorKind::Fail => write!(f, "failure"),
            ErrorKind::InvalidParameter => write!(f, "invalid parameter"),
            ErrorKind::NoMemory => write!(f, "out of memory"),
            ErrorKind::BufferTooSmall => write!(f, "buffer too small"),
            ErrorKind::DataCorrupt => write!(f, "corrupted data"),
            ErrorKind::InvalidSignature => write!(f, "invalid signature"),
            ErrorKind::NotSupported => write!(f, "operation not supported"),

            ErrorKind::SessionSendOutputToPeer => write!(f, "send key agreement data to peer"),
            ErrorKind::SessionKeyAgreementNotFinished => write!(f, "key agreement not finished"),
            ErrorKind::SessionTransportError => write!(f, "transport layer error"),
            ErrorKind::SessionGetPublicKeyForIdError => {
                write!(f, "failed to get public key for ID")
            }

            ErrorKind::CompareSendOutputToPeer => write!(f, "send comparison data to peer"),
            ErrorKind::CompareMatch => write!(f, "data matches"),
            ErrorKind::CompareNoMatch => write!(f, "data does not match"),
            ErrorKind::CompareNotReady => write!(f, "comparator not ready"),
        }
    }
}

/// A list of Themis error categories.
///
/// This enumeration is used by [`Error`] type, returned by most Themis functions. Some error kinds
/// are specific to particular functions, and some are used internally by the library.
///
/// [`Error`]: struct.Error.html
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum ErrorKind {
    /// Catch-all generic error.
    ///
    /// If you encounter this error kind then the Themis binding is likely to be out of sync with
    /// the core library. The contained error code has not been mapped onto `ErrorKind` value.
    #[doc(hidden)]
    UnknownError(i32),
    /// "Fatal error: success!"
    ///
    /// This value is used internally to distinguish successful function calls conveniently.
    /// End-users should never encounter it.
    #[doc(hidden)]
    Success,

    /// General failure.
    Fail,
    /// Some input parameter has incorrect value.
    InvalidParameter,
    /// Could not allocate memory.
    NoMemory,
    /// The provided buffer is too small to fit the result.
    BufferTooSmall,
    /// Input data is corrupted.
    DataCorrupt,
    /// Input data contains invalid signature.
    InvalidSignature,
    /// Operation not supported.
    NotSupported,

    /// Send output with internal data of Secure Session to the peer.
    ///
    /// This is not actually an error and the end-user should never see it.
    #[doc(hidden)]
    SessionSendOutputToPeer,
    /// Attempt to use Secure Session before completing key exchange.
    SessionKeyAgreementNotFinished,
    /// Transport layer returned error.
    SessionTransportError,
    /// Could not retrieve a public key corresponding to peer ID.
    SessionGetPublicKeyForIdError,

    /// Send output with internal data of Secure Comparator to the peer.
    ///
    /// This is not actually an error and the end-user should never see it.
    #[doc(hidden)]
    CompareSendOutputToPeer,
    /// Indicates that compared data matches.
    ///
    /// This is not actually an error and the end-user should never see it.
    #[doc(hidden)]
    CompareMatch,
    /// Indicates that compared data does not match.
    ///
    /// This is not actually an error and the end-user should never see it.
    #[doc(hidden)]
    CompareNoMatch,
    /// Attempt to use Secure Comparator before completing nonce exchange.
    CompareNotReady,
}
