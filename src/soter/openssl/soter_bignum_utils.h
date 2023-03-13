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

#ifndef THEMIS_SOTER_BIGNUM_UTILS_H
#define THEMIS_SOTER_BIGNUM_UTILS_H

#include <openssl/bn.h>
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/params.h>
#endif

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

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
static int get_bn_param(
    const EVP_PKEY* pkey, const char* name, unsigned char* buf, size_t buf_size, BIGNUM** bn)
{
    OSSL_PARAM params[2];

    params[0] = OSSL_PARAM_construct_BN(name, buf, buf_size);
    params[1] = OSSL_PARAM_construct_end();

    if (!EVP_PKEY_get_params(pkey, params)) {
        return 0;
    }

    return OSSL_PARAM_get_BN(&params[0], bn);
}
#endif

#endif /* THEMIS_SOTER_BIGNUM_UTILS_H */
