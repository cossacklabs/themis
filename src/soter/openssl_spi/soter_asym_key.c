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
  if(!key || key_length<sizeof(soter_container_hdr_t)){
    return 0;
  }
  return (*(int32_t*)(key))&0xffffff00;
}

int32_t soter_key_get_length_id(const uint8_t* key, const size_t key_length){
  if(!key || key_length<sizeof(soter_container_hdr_t)){
    return 0;
  }
  return (int32_t)(key[3])-0x30;
}

soter_status_t soter_key_pair_gen(int32_t alg_id, uint8_t* private_key, size_t* private_key_length, uint8_t* public_key, size_t* public_key_length){
  soter_status_t res = SOTER_SUCCESS;
  soter_crypter_param_t* pm=map_shm(SOTER_CRYPTER_MAIN_SHM, sizeof(soter_crypter_param_t));
  if(!pm){
    return SOTER_FAIL;
  }
  pm->op_code=ASYM_GEN;
  pm->param1_len=sizeof(int32_t);
  pm->param2_len=0;
  pm->param3_len=0;
  int32_t* p=map_shm(SOTER_CRYPTER_PARAM_SHM, sizeof(int32_t));
  if(!p){
    return SOTER_FAIL;    
  }
  *p=alg_id;
  set_sem();
  wait_sem();
  res=pm->op_code;
  *private_key_length=pm->param1_len;
  *public_key_length=pm->param2_len;
  if(SOTER_SUCCESS==res){
    uint8_t* r=map_shm(SOTER_CRYPTER_MAIN_SHM, pm->param1_len+pm->param2_len);
    if(!r){
      unmap_shm(p, sizeof(int32_t));
      return HM_FAIL;
    }
    memcpy(private_key, r, pm->param1_len);
    memcpy(public_key, r+pm->param1_len, pm->param2_len);
    unmap_shm(r, pm->param1_len+pm->param2_len);
  }
  return res;
}


