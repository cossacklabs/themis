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

#include <soter/error.h>
#include <soter/soter.h>

#ifdef LIBRESSL
#include <soter/openssl/soter_openssl.h>
#endif

#ifdef OPENSSL
#include <soter/openssl/soter_openssl.h>
#endif

#include <soter/soter_sign_rsa.h>
#include <soter/soter_sign_ecdsa.h>

#define SOTER_SIGN_ALGS \
    SOTER_SIGN_ALG(rsa,pss,pkcs8)	\
    SOTER_SIGN_ALG(ecdsa,none,pkcs8)

#define SOTER_SIGN_ALG(alg, padding, kdf)				\
  case SOTER_SIGN_##alg##_##padding##_##kdf :				\
  return soter_sign_init_##alg##_##padding##_##kdf(ctx,private_key,private_key_length, public_key, public_key_length);
soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, soter_sign_alg_t algId, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=algId;
  switch(algId){

    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG


#define SOTER_SIGN_ALG(alg, padding, kdf)				\
  case SOTER_SIGN_##alg##_##padding##_##kdf :				\
  return soter_verify_init_##alg##_##padding##_##kdf(ctx,private_key,private_key_length, public_key, public_key_length);
soter_status_t soter_verify_init(soter_sign_ctx_t* ctx, soter_sign_alg_t algId, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=algId;
  switch(algId){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)				\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_export_key_##alg##_##padding##_##kdf(ctx,key,key_length,isprivate);
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG


#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_update_##alg##_##padding##_##kdf(ctx,data,data_length);
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  if(!ctx||!data||!data_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_verify_update_##alg##_##padding##_##kdf(ctx,data,data_length);
soter_status_t soter_verify_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  if(!ctx||!data||!data_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_final_##alg##_##padding##_##kdf(ctx,signature,signature_length);
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_verify_final_##alg##_##padding##_##kdf(ctx,signature,signature_length);
soter_status_t soter_verify_final(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length){
  if(!ctx||!signature||!signature_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG


soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  soter_sign_ctx_t* ctx=malloc(sizeof(soter_sign_ctx_t));
  if(!ctx){
    return NULL;
  }
  if(soter_sign_init(ctx, alg, private_key, private_key_length, public_key, public_key_length)!=SOTER_SUCCESS){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_sign_ctx_t* soter_verify_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length){
  soter_sign_ctx_t* ctx=malloc(sizeof(soter_sign_ctx_t));
  if(!ctx){
    return NULL;
  }
  if(soter_verify_init(ctx, alg, private_key, private_key_length, public_key, public_key_length)!=SOTER_SUCCESS){
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }    
  if(ctx->pkey_ctx){
    EVP_PKEY_CTX_free(ctx->pkey_ctx);
    ctx->pkey_ctx=NULL;
  }
  free(ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_verify_destroy(soter_sign_ctx_t* ctx){
  return soter_sign_destroy(ctx);
}

soter_sign_alg_t soter_sign_get_alg_id(soter_sign_ctx_t* ctx)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  return ctx->alg;
}

soter_sign_alg_t soter_verify_get_alg_id(soter_verify_ctx_t* ctx)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  return ctx->alg;
}
