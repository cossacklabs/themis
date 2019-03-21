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

//! Miscellaneous utilities.
//!
//! This module contains various small utilities used across several modules.

use std::ptr;

/// Splits a slice into raw pointer and length for C code to use.
pub fn into_raw_parts(slice: &[u8]) -> (*const u8, usize) {
    let len = slice.len();
    let ptr = if len == 0 {
        ptr::null()
    } else {
        slice.as_ptr()
    };
    (ptr, len)
}
