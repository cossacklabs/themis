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

#include <assert.h>

#include <soter/soter_asym_key.h>
#include <soter/soter_container.h>
#include "soter_engine_consts.h"
#include "soter_rsa_key.h"
#include "soter_rsa_key_pair_gen.h"
#include "soter_ec_key.h"
#include "soter_ec_key_pair_gen.h"

bool soter_key_is_private(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  assert(key[0]=='R' || key[0]=='U');
  return (key[0]=='R')?true:false;
}

int32_t soter_key_get_alg_id(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  return (*(int32_t*)(key))&0x00ffffff;
}

int32_t soter_key_get_length_id(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  return (int32_t)(key[3]);
}

#define SOTER_RSA_KEY_GEN                       \
  SOTER_RSA_KEY_GEN_(1024)                      \
  SOTER_RSA_KEY_GEN_(2048)                      \
  SOTER_RSA_KEY_GEN_(4096)                      \
  SOTER_RSA_KEY_GEN_(8192)                      
  
#define SOTER_RSA_KEY_GEN_(len)                                         \
  case (SOTER_ASYM_RSA|SOTER_ASYM_RSA_LENGTH_##len):{                   \
    if(!private_key || !public_key || (*private_key_length)<sizeof(soter_rsa_priv_key_##len##_t) || (*public_key_length)<sizeof(soter_rsa_pub_key_##len##_t)){ \
      (*private_key_length) = sizeof(soter_rsa_priv_key_##len##_t);     \
      (*public_key_length) = sizeof(soter_rsa_pub_key_##len##_t);       \
      return SOTER_BUFFER_TOO_SMALL;                                    \
    }                                                                   \
    soter_rsa_key_pair_gen_t* pair = soter_rsa_key_pair_gen_create(RSA_KEY_LENGTH_##len); \
    if(!pair){                                                          \
      return SOTER_FAIL;                                                \
    }                                                                   \
    if(SOTER_SUCCESS != soter_rsa_key_pair_gen_export_key(pair, private_key, private_key_length, true)){ \
      soter_rsa_key_pair_gen_destroy(pair);                             \
      return SOTER_FAIL;                                                \
    }                                                                   \
    if(SOTER_SUCCESS != soter_rsa_key_pair_gen_export_key(pair, public_key, public_key_length, false)){ \
      soter_rsa_key_pair_gen_destroy(pair);                             \
      return SOTER_FAIL;                                                \
    }                                                                   \
    soter_rsa_key_pair_gen_destroy(pair);                               \
    return SOTER_SUCCESS;                                               \
  }

#define SOTER_EC_KEY_GEN                      \
  SOTER_EC_KEY_GEN_(256)                      \
  SOTER_EC_KEY_GEN_(384)                      \
  SOTER_EC_KEY_GEN_(521)                      
  
#define SOTER_EC_KEY_GEN_(len)                                          \
  case (SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_##len):{                     \
    if(!private_key || !public_key || (*private_key_length)<sizeof(soter_ec_priv_key_##len##_t) || (*public_key_length)<sizeof(soter_ec_pub_key_##len##_t)){ \
      (*private_key_length) = sizeof(soter_ec_priv_key_##len##_t);      \
      (*public_key_length) = sizeof(soter_ec_pub_key_##len##_t);        \
      return SOTER_BUFFER_TOO_SMALL;                                    \
    }                                                                   \
    soter_ec_key_pair_gen_t* pair = soter_ec_key_pair_gen_create(SOTER_ASYM_EC_LENGTH_##len); \
    if(!pair){                                                          \
      return SOTER_FAIL;                                                \
    }                                                                   \
    if(SOTER_SUCCESS != soter_ec_key_pair_gen_export_key(pair, private_key, private_key_length, true)){ \
      soter_ec_key_pair_gen_destroy(pair);                              \
      return SOTER_FAIL;                                                \
    }                                                                   \
    if(SOTER_SUCCESS != soter_ec_key_pair_gen_export_key(pair, public_key, public_key_length, false)){ \
      soter_ec_key_pair_gen_destroy(pair);                              \
      return SOTER_FAIL;                                                \
    }                                                                   \
    soter_ec_key_pair_gen_destroy(pair);                                \
    return SOTER_SUCCESS;                                               \
  }

soter_status_t soter_key_pair_gen(int32_t alg_id, uint8_t* private_key, size_t* private_key_length, uint8_t* public_key, size_t* public_key_length){
  soter_status_t res = SOTER_SUCCESS;
  switch(alg_id&(SOTER_ASYM_ALGS|SOTER_ASYM_KEY_LENGTH)){
    SOTER_RSA_KEY_GEN
    SOTER_EC_KEY_GEN
  default:
    res = SOTER_INVALID_PARAMETER;
  }
  return res;
}


