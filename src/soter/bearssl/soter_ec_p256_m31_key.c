/*
 * Copyright (c) 2015 Cossack Labs Limited
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

#include "soter_ec_p256_m31_key.h"

#include "soter_engine_consts.h"

soter_status_t soter_ec_p256_m31_pub_key_to_byte_array(soter_ec_p256_m31_pub_key_t* from, uint8_t* to, size_t* to_len){
  SOTER_CHECK_IN_PARAM(from);
  SOTER_CHECK_IN_PARAM(SOTER_ASYM_EC_P256_M31==soter_key_get_alg_id((uint8_t*)from, sizeof(soter_ec_p256_m31_pub_key_t)));
  SOTER_CHECK_IN_PARAM(!soter_key_is_private((uint8_t*)from, sizeof(soter_ec_p256_m31_pub_key_t)));
  SOTER_CHECK_OUT_BUF_PARAM(to, to_len, sizeof(soter_ec_p256_m31_pub_key_t));
  if(SOTER_SUCCESS!=soter_update_container_checksum((soter_container_hdr_t*)from)){
    return SOTER_FAIL;
  }
  memcpy(to, (uint8_t*)from, sizeof(soter_ec_p256_m31_pub_key_t));
  *to_len=sizeof(soter_ec_p256_m31_pub_key_t);
  return SOTER_SUCCESS;
}

soter_status_t soter_ec_p256_m31_byte_array_to_pub_key(const uint8_t* key, const size_t key_length, soter_ec_p256_m31_pub_key_t* to){
  SOTER_CHECK_IN_PARAM(to);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(key, key_length);
  SOTER_CHECK_IN_PARAM(key_length>sizeof(soter_container_hdr_t));
  SOTER_CHECK_IN_PARAM(key_length==ntohl(((soter_container_hdr_t*)key)->size));
  SOTER_CHECK_IN_PARAM(SOTER_SUCCESS==soter_verify_container_checksum((soter_container_hdr_t*)key));
  SOTER_CHECK_IN_PARAM(SOTER_ASYM_EC_P256_M31==soter_key_get_alg_id(key, key_length));
  SOTER_CHECK_IN_PARAM(!soter_key_is_private(key, key_length));

  memcpy((void*)to, key, key_length);
  to->impl.q = to->key;
  
  return SOTER_SUCCESS;
}

soter_status_t soter_ec_p256_m31_priv_key_to_byte_array(soter_ec_p256_m31_priv_key_t* from, uint8_t* to, size_t* to_len){
  SOTER_CHECK_IN_PARAM(from);
  SOTER_CHECK_IN_PARAM(SOTER_ASYM_EC_P256_M31==soter_key_get_alg_id((uint8_t*)from, sizeof(soter_ec_p256_m31_priv_key_t)));
  SOTER_CHECK_IN_PARAM(!soter_key_is_private((uint8_t*)from, sizeof(soter_ec_p256_m31_priv_key_t)));
  SOTER_CHECK_OUT_BUF_PARAM(to, to_len, sizeof(soter_ec_p256_m31_priv_key_t));
  if(SOTER_SUCCESS!=soter_update_container_checksum((soter_container_hdr_t*)from)){
    return SOTER_FAIL;
  }
  memcpy(to, (uint8_t*)from, sizeof(soter_ec_p256_m31_priv_key_t));
  *to_len=sizeof(soter_ec_p256_m31_priv_key_t);
  return SOTER_SUCCESS;  
}

soter_status_t soter_ec_p256_m31_byte_array_to_priv_key(const uint8_t* key, const size_t key_length, soter_ec_p256_m31_priv_key_t* to){
  SOTER_CHECK_IN_PARAM(to);
  SOTER_CHECK_IN_BUF_PARAM_NON_EMPTY(key, key_length);
  SOTER_CHECK_IN_PARAM(key_length>sizeof(soter_container_hdr_t));
  SOTER_CHECK_IN_PARAM(key_length==ntohl(((soter_container_hdr_t*)key)->size));
  SOTER_CHECK_IN_PARAM(SOTER_SUCCESS==soter_verify_container_checksum((soter_container_hdr_t*)key));
  SOTER_CHECK_IN_PARAM(SOTER_ASYM_EC_P256_M31==soter_key_get_alg_id(key, key_length));
  SOTER_CHECK_IN_PARAM(soter_key_is_private(key, key_length));

  memcpy((void*)to, key, key_length);
  to->impl.x = to->key;
  
  return SOTER_SUCCESS;
}
