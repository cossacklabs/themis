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

#include "soter/error.h"
#include "soter/soter.h"
#include "soter_engine.h"
#include "soter_engine_consts.h"

#include <assert.h>

size_t soter_hash_block_size(int32_t algo){
    switch (algo){
    case SOTER_HASH_SHA_256:
      return SOTER_HASH_SHA_256_BLOCK_SIZE;
    }
    return 0;
}

size_t soter_hash_length(int32_t algo){
  switch (algo){
  case SOTER_HASH_SHA_256:
    return SOTER_HASH_SHA_256_LENGTH;
  }
  return 0;
}


soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, int32_t alg){
  SOTER_CHECK_IN_PARAM(hash_ctx);
  SOTER_CHECK_IN_PARAM(0!=soter_hash_block_size(alg));
  hash_ctx->alg=alg;
  switch(alg){
  case SOTER_HASH_SHA_256:
    br_sha256_init(&(hash_ctx->impl.sha256));
    return SOTER_SUCCESS;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_FAIL;
}
  

soter_status_t soter_hash_update(soter_hash_ctx_t *hash_ctx, const void *data, size_t length){
  SOTER_CHECK_IN_PARAM(hash_ctx);
  SOTER_CHECK_IN_PARAM(data);
  switch(hash_ctx->alg){
  case SOTER_HASH_SHA_256:
    br_sha256_update(&(hash_ctx->impl.sha256), data, length);
    return SOTER_SUCCESS;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_FAIL;  
}

soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t* hash_value, size_t* hash_length){
  SOTER_CHECK_IN_PARAM(hash_ctx);
  SOTER_CHECK_IN_PARAM(hash_length);
  SOTER_CHECK_OUT_BUF_PARAM(hash_value, hash_length, soter_hash_length(hash_ctx->alg));
  switch(hash_ctx->alg){
  case SOTER_HASH_SHA_256:
    br_sha256_out(&(hash_ctx->impl.sha256), hash_value);
    *hash_length=soter_hash_length(hash_ctx->alg);
    return SOTER_SUCCESS;
  default:
    return SOTER_INVALID_PARAMETER;
  }
  return SOTER_FAIL;  
}

soter_hash_ctx_t* soter_hash_create(int32_t algo){
  soter_status_t status;
  soter_hash_ctx_t *ctx = malloc(sizeof(soter_hash_ctx_t));
  assert(ctx);
  if(SOTER_SUCCESS != soter_hash_init(ctx, algo)){
    soter_hash_destroy(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_hash_cleanup(soter_hash_ctx_t* hash_ctx){
  SOTER_CHECK_IN_PARAM(hash_ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_hash_destroy(soter_hash_ctx_t *hash_ctx){
  SOTER_CHECK_IN_PARAM(hash_ctx);
  if(SOTER_SUCCESS != soter_hash_cleanup(hash_ctx)){
    return SOTER_FAIL;
  }
  free(hash_ctx);
  return SOTER_SUCCESS;
}
