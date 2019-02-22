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

use std::ptr;

#[test]
fn check_ffi_call() {
    let mut private_key_len = 0;
    let mut public_key_len = 0;
    let status = unsafe {
        libthemis_sys::themis_gen_ec_key_pair(
            ptr::null_mut(),
            &mut private_key_len,
            ptr::null_mut(),
            &mut public_key_len,
        )
    };
    assert_eq!(
        status,
        libthemis_sys::THEMIS_BUFFER_TOO_SMALL as libthemis_sys::themis_status_t
    );
}
