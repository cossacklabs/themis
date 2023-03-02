/*
* Copyright (c) 2023 Cossack Labs Limited
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

#ifndef THEMIS_SOTER_EC_KEY_UTILS_H
#define THEMIS_SOTER_EC_KEY_UTILS_H

#include <string.h>

#include <openssl/bn.h>
#include <openssl/obj_mac.h>

#include "soter/soter_ec_key.h"

static bool is_curve_supported(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1:
    case NID_secp384r1:
    case NID_secp521r1:
        return true;
    default:
        return false;
    }
}

/* Input size directly since public key type structures may be aligned to word boundary */
static size_t ec_pub_key_size(int curve, bool compressed)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(256) : EC_PUB_UNCOMPRESSED_SIZE(256));
    case NID_secp384r1: /* P-384 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(384) : EC_PUB_UNCOMPRESSED_SIZE(384));
    case NID_secp521r1: /* P-521 */
        return sizeof(soter_container_hdr_t)
               + (compressed ? EC_PUB_SIZE(521) : EC_PUB_UNCOMPRESSED_SIZE(521));
    default:
        return 0;
    }
}

static size_t ec_priv_key_size(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return sizeof(soter_ec_priv_key_256_t);
    case NID_secp384r1: /* P-384 */
        return sizeof(soter_ec_priv_key_384_t);
    case NID_secp521r1: /* P-521 */
        return sizeof(soter_ec_priv_key_521_t);
    default:
        return 0;
    }
}

static char* ec_pub_key_tag(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return EC_PUB_KEY_TAG(256);
    case NID_secp384r1: /* P-384 */
        return EC_PUB_KEY_TAG(384);
    case NID_secp521r1: /* P-521 */
        return EC_PUB_KEY_TAG(521);
    default:
        return NULL;
    }
}

static char* ec_priv_key_tag(int curve)
{
    switch (curve) {
    case NID_X9_62_prime256v1: /* P-256 */
        return EC_PRIV_KEY_TAG(256);
    case NID_secp384r1: /* P-384 */
        return EC_PRIV_KEY_TAG(384);
    case NID_secp521r1: /* P-521 */
        return EC_PRIV_KEY_TAG(521);
    default:
        return NULL;
    }
}

#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* Simple implementation for OpenSSL <1.1.0 where this function is missing */
static int BN_bn2binpad(const BIGNUM* a, unsigned char* to, int tolen)
{
    int bn_size = BN_num_bytes(a);
    int bytes_copied;

    if (a == NULL || to == NULL) {
        return -1;
    }

    if (tolen < bn_size) {
        return -1;
    }

    bytes_copied = BN_bn2bin(a, to + (tolen - bn_size));

    if (bytes_copied != bn_size) {
        return -1;
    }

    memset(to, 0, (size_t)(tolen - bn_size));

    return tolen;
}
#endif

#endif /* THEMIS_SOTER_EC_KEY_UTILS_H */
