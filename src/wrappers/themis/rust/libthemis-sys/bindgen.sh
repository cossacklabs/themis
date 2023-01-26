#!/usr/bin/env bash
#
# Generate "src/lib.rs" file with bindings
#
# Run this script from "libthemis-sys" directory before release
# to update raw FFI bindings with newly added functions and types:
#
#     ./bindgen.sh
#
# You need to have Bindgen, LLVM, rustfmt installed to run this script.
# Bindgen can be installed with
#
#     cargo install bindgen-cli
#
# rustfmt can be installed with
#
#     rustup component add rustfmt
#
# Suitable LLVM can usually be installed from your system's repositories.

set -e -o pipefail

# This is a pattern for what we export from libthemis-sys.
# Bindgen sees Soter as well as some system libraries, we don't need that.
WHITELIST="(THEMIS|themis|secure_(comparator|session)|STATE)_.*"

# Currently, we don't pass --target since none of the symbols we're linking
# against are architecture-specific. If this ever becomes a problem, then the
# thing to do is to split the generated code into different files for different
# platforms (like themis_x86_64.rs, themis_arm64.rs, etc.) and conditionally
# compile them depending on target.
#
# Though, we do disable layout tests which *are* architecture-specific and
# we cannot include them without breaking either 32-bit or 64-bit machines.
bindgen bindgen.h \
    --no-doc-comments \
    --no-layout-tests \
    --disable-header-comment \
    --rustified-enum "themis_key_kind" \
    --size_t-is-usize \
    --allowlist-function "$WHITELIST" \
    --allowlist-type "$WHITELIST" \
    --allowlist-var "$WHITELIST" \
    --output src/lib.rs \
    -- \
    -I ../../../../../include \
    -I ../../../../../src

TMP="$(mktemp)"

# Prepend copyright comment, #[allow] for various warnings we don't care about.
(cat << EOF
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
// by bindgen.
#![allow(clippy::all)]

// DO NOT EDIT.
//
// This file is automatically generated by "bindgen.sh".
// Instead of editing it by hand, rerun the script.

EOF

cat src/lib.rs) \
| rustfmt > "$TMP"
mv "$TMP" src/lib.rs
