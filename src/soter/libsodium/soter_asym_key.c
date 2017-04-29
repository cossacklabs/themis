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
#include "soter_x25519_key_pair_gen.h"
#include "soter_ed25519_key_pair_gen.h"

bool soter_key_is_private(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  assert(key[0]=='R' || key[0]=='U');
  return (key[0]=='R')?true:false;
}

int32_t soter_key_get_alg_id(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  if(0 == memcmp(key+1, "X2", 2)){
    return SOTER_ASYM_X25519;
  }
  if (0 == memcmp(key+1, "ED", 2)){
    return SOTER_ASYM_ED25519;
  }
  return 0;
}

int32_t soter_key_get_length_id(const uint8_t* key, const size_t key_length){
  assert(key && key_length>sizeof(soter_container_hdr_t));
  return (int32_t)(key[3]);
}

soter_status_t soter_key_pair_gen(int32_t alg_id, uint8_t* private_key, size_t* private_key_length, uint8_t* public_key, size_t* public_key_length){
  soter_status_t res = SOTER_SUCCESS;
  switch(alg_id){
  case SOTER_ASYM_X25519:{
    soter_x25519_key_pair_gen_t* pair=soter_x25519_key_pair_gen_create();
    if(!pair){
      return SOTER_FAIL;
    }
    soter_status_t res1=soter_x25519_key_pair_gen_export_key(pair, private_key, private_key_length, true);
    soter_status_t res2=soter_x25519_key_pair_gen_export_key(pair, public_key, public_key_length, false);
    soter_x25519_key_pair_gen_destroy(pair);
    return (res1<res2)?res2:res1;
  }
    break;
  case SOTER_ASYM_ED25519:{
    soter_ed25519_key_pair_gen_t* pair=soter_ed25519_key_pair_gen_create();
    if(!pair){
      return SOTER_FAIL;
    }
    soter_status_t res1=soter_ed25519_key_pair_gen_export_key(pair, private_key, private_key_length, true);
    soter_status_t res2=soter_ed25519_key_pair_gen_export_key(pair, public_key, public_key_length, false);
    soter_ed25519_key_pair_gen_destroy(pair);
    return (res1<res2)?res2:res1;
  }
    break;
  default:
    res = SOTER_INVALID_PARAMETER;
  }
  return res;
}


