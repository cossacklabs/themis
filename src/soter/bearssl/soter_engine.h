/*
* Copyright (c) 2017 Cossack Labs Limited
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

#ifndef SOTER_ENGINE_H
#define SOTER_ENGINE_H

#include "soter_engine_consts.h"

#include <soter/soter.h>
#include <bearssl.h>
#include "soter_ec_p256_m31_key.h"

struct soter_hash_ctx_type
{
  uint32_t alg;
  union{
    br_sha256_context sha256;
  } impl;
};

struct soter_sym_ctx_type{
  uint32_t alg;
  union{
    br_aes_big_ctr_keys aes_ctr_256;
  } impl;
  union{
    uint8_t aes_ctr_256[SOTER_SYM_AES_CTR_256_IV_SIZE];
  }nonce;
};

struct soter_sym_aead_ctx_type{
  uint32_t alg;
  soter_sym_ctx_t cypher;
  bool state;
  union{
    uint8_t aes_gcm_256[SOTER_SYM_AEAD_AES_GCM_256_AUTH_TAG_SIZE];
  }tag;
  union{
    uint8_t aes_gcm_256[SOTER_SYM_AEAD_AES_GCM_256_AUTH_TAG_SIZE];
  }h;
};

struct soter_asym_cipher_type{
  uint32_t alg;
};

struct soter_asym_ka_type{
  union{
    soter_ec_p256_m31_priv_key_t ec_p256_m31;
  }pk;
};

struct soter_sign_ctx_type{
  uint32_t alg;
  union{
    union{
      soter_ec_p256_m31_priv_key_t ec_p256_m31;
    }sk;
    union{
      soter_ec_p256_m31_pub_key_t ec_p256_m31;
    }pk;
  }key;
  soter_hash_ctx_t hash;
};

#endif /* SOTER_ENGINE_H */

