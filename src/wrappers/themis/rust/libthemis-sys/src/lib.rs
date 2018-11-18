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

//! Raw FFI bindings to libthemis.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
// For some weird reasons Clippy gets run on this crate as it's a path dependency of themis.
// This should not happen (see https://github.com/rust-lang-nursery/rust-clippy/issues/1066)
// but until that's fixed again, disable all lints which get triggered by the code generated
// by bindgen. We're using the stable version of compiler and I don't want to pull nightly
// just for clippy, so we have to enumerate all lints explicitly instead of "clippy::all".
#![cfg_attr(feature = "cargo-clippy", allow(const_static_lifetime))]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
