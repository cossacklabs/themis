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

//! Wrapper header for bindgen containing all Themis API declarations.

#include <themis/themis.h>

// TODO: move shims into Themis core
//
// These shims are here because they use C macros which are not exported by bindgen.
// Ideally, Themis should provide this functionality, but it's too unstable now
// for inclusion into the core library.

#include <stddef.h>
#include <stdint.h>

enum themis_key_kind
{
    THEMIS_KEY_INVALID,
    THEMIS_KEY_RSA_PRIVATE,
    THEMIS_KEY_RSA_PUBLIC,
    THEMIS_KEY_EC_PRIVATE,
    THEMIS_KEY_EC_PUBLIC,
};

/// Checks if the buffer contains a valid Themis key.
themis_status_t themis_is_valid_key(const uint8_t *key, size_t length);

/// Extracts the presumed key kind from the buffer.
enum themis_key_kind themis_get_key_kind(const uint8_t *key, size_t length);
