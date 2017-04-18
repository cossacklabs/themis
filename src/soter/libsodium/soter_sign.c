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

#include <assert.h>

#include <soter/error.h>
#include <soter/soter.h>

#include "soter_engine.h"

#include <soter/soter_container.h>
#include <soter/soter_asym_key.h>
#include <soter/soter_asym_sign.h>
#include "soter_sign_rsa.h"
#include "soter_sign_ecdsa.h"
#include "soter_engine_consts.h"

soter_status_t soter_sign(const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length, const uint8_t* data, const size_t data_length, uint8_t* signature, size_t* signature_length){
  soter_sign_ctx_t ctx;
  soter_status_t res = SOTER_SUCCESS;
  res = soter_sign_init(&ctx, private_key, private_key_length, public_key, public_key_length) ;
  if(SOTER_SUCCESS == res){
    res = soter_sign_update(&ctx, data, data_length);
    if(SOTER_SUCCESS == res){
      res = soter_sign_final(&ctx, signature, signature_length);
    }
  }
  soter_sign_cleanup(&ctx);
  return res;
}

soter_status_t soter_verify(const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length, const int8_t* data, const size_t data_length, const int8_t* signature, const size_t signature_length){
  soter_verify_ctx_t ctx;
  soter_status_t res = SOTER_SUCCESS;
  res = soter_verify_init(&ctx, private_key, private_key_length, public_key, public_key_length) ;
  if(SOTER_SUCCESS == res){
    res = soter_verify_update(&ctx, data, data_length);
    if(SOTER_SUCCESS == res){
      res = soter_verify_final(&ctx, signature, signature_length);
    }
  }
  soter_verify_cleanup(&ctx);
  return res;
}


soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_verify_init(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_verify_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_verify_final(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sign_cleanup(soter_sign_ctx_t* ctx){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_verify_cleanup(soter_sign_ctx_t* ctx){
  return SOTER_NOT_SUPPORTED;
}

soter_sign_ctx_t* soter_sign_create(const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  return SOTER_NOT_SUPPORTED;
}

soter_sign_ctx_t* soter_verify_create(const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx){
  return SOTER_NOT_SUPPORTED;
}

soter_status_t soter_verify_destroy(soter_sign_ctx_t* ctx){
  return SOTER_NOT_SUPPORTED;
}