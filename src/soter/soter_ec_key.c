/*
 * Copyright (c) 2019 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "soter/soter_ec_key.h"

#include <soter/soter_api.h>

/*
 * EC data is appended directly to soter_container_hdr_t when exporting EC keys,
 * so that's the length we should expect. sizeof(soter_ec_pub_key_256_t) structs
 * may be slightly bigger than required size due compiler alignment, so we check
 * the length directly instead of comparing to sizeof (like we do with RSA keys).
 */

SOTER_PRIVATE_API
soter_status_t soter_ec_pub_key_check_length(const soter_container_hdr_t* key, size_t key_length)
{
    /*
     * Backends can import both compressed and uncompressed public keys.
     * Themis exports public keys in compressed format, but Themis NG
     * uses uncompressed one. Make sure to accept both.
     */
    switch (key->tag[3]) {
    case EC_SIZE_TAG_256:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(256)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_UNCOMPRESSED_SIZE(256)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;

    case EC_SIZE_TAG_384:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(384)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_UNCOMPRESSED_SIZE(384)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;

    case EC_SIZE_TAG_521:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(521)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_UNCOMPRESSED_SIZE(521)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}

SOTER_PRIVATE_API
soter_status_t soter_ec_priv_key_check_length(const soter_container_hdr_t* key, size_t key_length)
{
    /*
     * Due to a historical mistake, EC private keys in Themis have the same
     * length as public keys, not EC_PRIV_SIZE(bits). That's one byte more
     * than necessary and that last byte is always zero.
     *
     * Themis NG uses proper serialization for private keys. Accept it too.
     */
    switch (key->tag[3]) {
    case EC_SIZE_TAG_256:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(256)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PRIV_SIZE(256)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;

    case EC_SIZE_TAG_384:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(384)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PRIV_SIZE(384)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;

    case EC_SIZE_TAG_521:
        if (key_length == sizeof(soter_container_hdr_t) + EC_PUB_SIZE(521)) {
            return SOTER_SUCCESS;
        }
        if (key_length == sizeof(soter_container_hdr_t) + EC_PRIV_SIZE(521)) {
            return SOTER_SUCCESS;
        }
        return SOTER_INVALID_PARAMETER;
    }
    return SOTER_INVALID_PARAMETER;
}
