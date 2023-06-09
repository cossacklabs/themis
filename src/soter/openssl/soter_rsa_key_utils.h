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

#ifndef THEMIS_SOTER_RSA_KEY_UTILS_H
#define THEMIS_SOTER_RSA_KEY_UTILS_H

#include "soter/soter_rsa_key.h"

#include <string.h>

static size_t rsa_pub_key_size(int mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return sizeof(soter_rsa_pub_key_1024_t);
    case 256: /* 2048 */
        return sizeof(soter_rsa_pub_key_2048_t);
    case 512: /* 4096 */
        return sizeof(soter_rsa_pub_key_4096_t);
    case 1024: /* 8192 */
        return sizeof(soter_rsa_pub_key_8192_t);
    default:
        return 0;
    }
}

static size_t rsa_priv_key_size(int mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return sizeof(soter_rsa_priv_key_1024_t);
    case 256: /* 2048 */
        return sizeof(soter_rsa_priv_key_2048_t);
    case 512: /* 4096 */
        return sizeof(soter_rsa_priv_key_4096_t);
    case 1024: /* 8192 */
        return sizeof(soter_rsa_priv_key_8192_t);
    default:
        return 0;
    }
}

static char* rsa_pub_key_tag(int mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return RSA_PUB_KEY_TAG(1024);
    case 256: /* 2048 */
        return RSA_PUB_KEY_TAG(2048);
    case 512: /* 4096 */
        return RSA_PUB_KEY_TAG(4096);
    case 1024: /* 8192 */
        return RSA_PUB_KEY_TAG(8192);
    default:
        return NULL;
    }
}

static char* rsa_priv_key_tag(int mod_size)
{
    switch (mod_size) {
    case 128: /* 1024 */
        return RSA_PRIV_KEY_TAG(1024);
    case 256: /* 2048 */
        return RSA_PRIV_KEY_TAG(2048);
    case 512: /* 4096 */
        return RSA_PRIV_KEY_TAG(4096);
    case 1024: /* 8192 */
        return RSA_PRIV_KEY_TAG(8192);
    default:
        return NULL;
    }
}

static bool is_mod_size_supported(int mod_size)
{
    switch (mod_size) {
    case 128:
    case 256:
    case 512:
    case 1024:
        return true;
    default:
        return false;
    }
}

#endif /* THEMIS_SOTER_RSA_KEY_UTILS_H */
