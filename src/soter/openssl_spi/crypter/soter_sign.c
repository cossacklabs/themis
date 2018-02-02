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

#ifdef CRYPTO_ENGINE_PATH
#define CEP <soter/CRYPTO_ENGINE_PATH/soter_engine.h>
#include CEP
#undef CEP
#else
#include <soter/openssl/soter_engine.h>
#endif

#include <soter/soter_container.h>
#include <soter/soter_asym_key.h>
#include <soter/soter_asym_sign.h>
#include "../soter_sign_rsa.h"
#include "../soter_sign_ecdsa.h"
#include "../soter_engine_consts.h"


#define SOTER_SIGN_ALGS                 \
    SOTER_SIGN_ALG(rsa,pss,pkcs8)	\
    SOTER_SIGN_ALG(ecdsa,none,pkcs8)

soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length)
{
  if(!ctx || !private_key ||  private_key_length<sizeof(soter_container_hdr_t) || !soter_key_is_private(private_key, private_key_length)){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=soter_key_get_alg_id(private_key, private_key_length);
  if(!(ctx->alg)){
    return SOTER_INVALID_PARAMETER;
  }
  switch((ctx->alg)&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA:
    return soter_sign_init_rsa_pss_pkcs8(ctx,private_key,private_key_length, NULL, 0);
  case SOTER_ASYM_EC:			     
    return soter_sign_init_ecdsa_none_pkcs8(ctx,private_key,private_key_length, NULL, 0);
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_init(soter_sign_ctx_t* ctx, const void* public_key, const size_t public_key_length)
{
  if(!ctx || !public_key ||  public_key_length<sizeof(soter_container_hdr_t) || soter_key_is_private(public_key, public_key_length)){
    return SOTER_INVALID_PARAMETER;
  }
  ctx->alg=soter_key_get_alg_id(public_key, public_key_length);
  if(!(ctx->alg)){
    return SOTER_INVALID_PARAMETER;
  }
  switch((ctx->alg)&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_verify_init_rsa_pss_pkcs8(ctx, NULL, 0, public_key, public_key_length);
  case SOTER_ASYM_EC :			     
    return soter_verify_init_ecdsa_none_pkcs8(ctx, NULL, 0, public_key, public_key_length);    
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  if(!ctx||!data||!data_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_sign_update_rsa_pss_pkcs8(ctx,data, data_length);
  case SOTER_ASYM_EC :			     
    return soter_sign_update_ecdsa_none_pkcs8(ctx, data, data_length);
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  if(!ctx||!data||!data_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_verify_update_rsa_pss_pkcs8(ctx,data, data_length);
  case SOTER_ASYM_EC :			     
    return soter_verify_update_ecdsa_none_pkcs8(ctx,data, data_length);    
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_sign_final_rsa_pss_pkcs8(ctx, signature, signature_length);
  case SOTER_ASYM_EC :			     
    return soter_sign_final_ecdsa_none_pkcs8(ctx, signature, signature_length);
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_final(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length){
  if(!ctx||!signature||!signature_length){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_verify_final_rsa_pss_pkcs8(ctx, signature, signature_length);
  case SOTER_ASYM_EC :			     
    return soter_verify_final_ecdsa_none_pkcs8(ctx, signature, signature_length);    
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_sign_cleanup(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_sign_cleanup_rsa_pss_pkcs8(ctx);
  case SOTER_ASYM_EC :			     
    return soter_sign_cleanup_ecdsa_none_pkcs8(ctx);
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}

soter_status_t soter_verify_cleanup(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  switch(ctx->alg&SOTER_ASYM_ALG_MASK){
  case SOTER_ASYM_RSA :			     
    return soter_verify_cleanup_rsa_pss_pkcs8(ctx);
  case SOTER_ASYM_EC :			     
    return soter_verify_cleanup_ecdsa_none_pkcs8(ctx);    
  default:
    return SOTER_INVALID_PARAMETER;
  };
  return SOTER_INVALID_PARAMETER;
}


soter_sign_ctx_t* soter_sign_create(const void* private_key, const size_t private_key_length){
  soter_sign_ctx_t* ctx=calloc(sizeof(soter_sign_ctx_t),1);
  assert(ctx);
  if(soter_sign_init(ctx, private_key, private_key_length)!=SOTER_SUCCESS){
    soter_sign_cleanup(ctx);
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_sign_ctx_t* soter_verify_create(const void* public_key, const size_t public_key_length){
  soter_sign_ctx_t* ctx=calloc(sizeof(soter_sign_ctx_t),1);
  assert(ctx);
  if(soter_verify_init(ctx, public_key, public_key_length)!=SOTER_SUCCESS){
    soter_verify_cleanup(ctx);
    free(ctx);
    return NULL;
  }
  return ctx;
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  soter_sign_cleanup(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

soter_status_t soter_verify_destroy(soter_sign_ctx_t* ctx){
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }    
  soter_verify_cleanup(ctx);
  free(ctx);
  return SOTER_SUCCESS;
}

int32_t soter_sign_get_alg_id(soter_sign_ctx_t* ctx)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  return ctx->alg;
}

int32_t soter_verify_get_alg_id(soter_verify_ctx_t* ctx)
{
  if(!ctx){
    return SOTER_INVALID_PARAMETER;
  }
  return ctx->alg;
}
