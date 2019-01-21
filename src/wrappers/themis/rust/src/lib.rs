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

//! # Themis Library
//!
//! **Themis** is a high-level cryptographic library.
//!
//! Themis allows you to protect data at rest (in your database, files, or wherever you store it
//! in your application) and data in motion (i.e. travelling between client and server, server
//! and server, application and application, a smart coffee machine and a smart vacuum cleaner).
//!
//! Themis is a portable, cross-platform implementation of several cryptosystems:
//!
//!   - **[Secure Message]:** general purpose cryptographic system aimed at securing
//!     communication between two parties;
//!
//!   - **[Secure Session]:** cryptographic system aimed at maintaining a messaging
//!     session between two peers, with the session-wise protection of their data exchange;
//!
//!   - **[Secure Cell]:** cryptographic system aimed at storing data encrypted with
//!     a symmetric key, with additional security guarantees;
//!
//!   - **[Secure Comparator]:** Zero-Knowledge Proof-based protocol built around SMP
//!     with ECC math, augmented with additional checks and features;
//!
//! ...all wrapped as nice high-level objects that are easy to use in your favourite environment.
//!
//! [Secure Message]: secure_message/index.html
//! [Secure Session]: secure_session/index.html
//! [Secure Cell]: secure_cell/index.html
//! [Secure Comparator]: secure_comparator/index.html

#![warn(missing_docs)]
#![doc(html_no_source)]
#![doc(html_logo_url = "https://rust-themis.ilammy.net/images/logo.png")]
#![doc(html_favicon_url = "https://rust-themis.ilammy.net/images/favicon.png")]

pub mod keygen;
pub mod keys;
pub mod secure_cell;
pub mod secure_comparator;
pub mod secure_message;
pub mod secure_session;

mod error;
mod utils;

pub use crate::error::{Error, ErrorKind, Result};
