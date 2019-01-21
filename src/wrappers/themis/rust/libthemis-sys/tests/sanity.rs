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

use std::ffi::CStr;

#[test]
fn check_version() {
    let version = unsafe { CStr::from_ptr(libthemis_sys::themis_version()) };
    // Themis 0.10.0 is slightly buggy and identifies itself as 0.9.
    assert!(version
        .to_str()
        .expect("valid UTF-8")
        .contains("themis 0.9"));
}
