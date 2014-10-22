/**
 * @file
 *
 * (c) CossackLabs
 */

#include <common/error.h>
#include <soter/soter.h>
#include <soter/openssl/soter_openssl.h>
#include <soter/soter_sign_rsa.h>
#include <soter/soter_sign_ecdsa.h>

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf :				\
  return soter_sign_init_##alg##_##padding##_##kdf(ctx,algId,key,key_length);
soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, soter_sign_alg_t algId, const void* key, const size_t key_length)
{
  switch(algId){
    SOTER_SIGN_ALGS
  };
  return HERMES_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_gen_key_##alg##_##padding##_##kdf(ctx);
soter_status_t soter_sign_gen_key(soter_sign_ctx_t* ctx){
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  };
  return HERMES_INVALID_PARAMETER;
}

#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_export_key_##alg##_##padding##_##kdf(ctx,key,key_length);
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length){
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  };
  return HERMES_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_update_##alg##_##padding##_##kdf(ctx,data,data_length);
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length){
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  };
}

#undef SOTER_SIGN_ALG

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  case SOTER_SIGN_##alg##_##padding##_##kdf:				\
  return soter_sign_final_##alg##_##padding##_##kdf(ctx,signature,signature_length);
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length){
  switch(ctx->alg){
    SOTER_SIGN_ALGS
  };
  return HERMES_INVALID_PARAMETER;
}
#undef SOTER_SIGN_ALG

soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg, const void* key, const size_t key_length){
}

soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx){

}
