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

#include "wrapper.h"

#include <string.h>

#include <arpa/inet.h>

#include <soter/soter_container.h>
#include <soter/soter_ec_key.h>
#include <soter/soter_rsa_key.h>

themis_status_t themis_is_valid_key(const uint8_t *key, size_t length)
{
    // FIXME: do something about alignment mismatch in Themis code
    //
    // Strictly speaking, this cast is actually an undefined behavior because `key` is not
    // guaranteed to be aligned for soter_container_hdr_t. The compiler actually produces
    // a warning for the cast. We silence it by using void*, just like the rest of C code
    // in Themis.
    //
    // The behavior is undefined because some architectures (most of the modern ones) allow
    // unaligned memory accesses. Other CPUs may issue an exception, which may be handled
    // by the OS, which may result in a SIGBUS being sent to the process, which usually
    // kills it. Or you may silently read data from a wrong memory address and that's it.
    //
    // This kinda tends to work because modern hardware and software is forgiving. Themis
    // is mostly used and tested on i386 and AMD64 which handle unaligned accesses just fine
    // (albeit slower). The next most popular architecture -- ARM -- used to have strict
    // alignment requirements, but they are significantly relaxed now on modern processors.
    // Furthermore, you need to get lucky with actually using an improperly unaligned pointer.
    // That's why you almost never observe any catastrophic results from such unsafe casts.
    //
    // In rust-themis we do our best to keep the pointers aligned. That's the reason for copying
    // the byte slices into a fresh new KeyBytes before passing them to any C function.
    //
    // However, this is still undefined behavior. I have killed yet another internet kitten
    // for the following line. I am sorry and ready to bear my sin.
    const soter_container_hdr_t *container = (const void*) key;

    if (!key || (length < sizeof(soter_container_hdr_t)))
    {
        return THEMIS_INVALID_PARAMETER;
    }

    if (length != ntohl(container->size))
    {
        return THEMIS_INVALID_PARAMETER;
    }

    if (SOTER_SUCCESS != soter_verify_container_checksum(container))
    {
        return THEMIS_DATA_CORRUPT;
    }

    return THEMIS_SUCCESS;
}

enum themis_key_kind themis_get_key_kind(const uint8_t *key, size_t length)
{
    if (!key || (length < sizeof(soter_container_hdr_t)))
    {
        return THEMIS_KEY_INVALID;
    }

    if (!memcmp(key, RSA_PRIV_KEY_PREF, strlen(RSA_PRIV_KEY_PREF)))
    {
        return THEMIS_KEY_RSA_PRIVATE;
    }
    if (!memcmp(key, RSA_PUB_KEY_PREF, strlen(RSA_PUB_KEY_PREF)))
    {
        return THEMIS_KEY_RSA_PUBLIC;
    }
    if (!memcmp(key, EC_PRIV_KEY_PREF, strlen(EC_PRIV_KEY_PREF)))
    {
        return THEMIS_KEY_EC_PRIVATE;
    }
    if (!memcmp(key, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
    {
        return THEMIS_KEY_EC_PUBLIC;
    }

    return THEMIS_KEY_INVALID;
}
