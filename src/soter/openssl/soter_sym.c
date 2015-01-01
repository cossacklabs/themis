/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include "common/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"
#include <openssl/err.h>


soter_status_t soter_pbkdf2(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length)
{
  if(!PKCS5_PBKDF2_HMAC((const char*)password, password_length, salt, salt_length, 0, EVP_sha256(), (*key_length), key))
    {
      return HERMES_FAIL;
    }
  return HERMES_SUCCESS;			
}

soter_status_t soter_nonkdf(const uint8_t* password, const size_t password_length, const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length)
{
  if(password_length<(*key_length)){
    return HERMES_BUFFER_TOO_SMALL;
  }
  memcpy(key,password,password_length);
  return HERMES_SUCCESS;
}

soter_status_t soter_set_auth_tag_gcm(soter_sym_ctx_t *sym_ctx, const unsigned char * tag, const size_t tag_length){
  if(!(tag_length==SOTER_SYM_AUTHTAG_LENGTH_gcm && EVP_CIPHER_CTX_ctrl(&(sym_ctx->evp_sym_ctx), EVP_CTRL_GCM_SET_TAG, SOTER_SYM_AUTHTAG_LENGTH_gcm, (void*)tag))){
    return HERMES_FAIL;
  }
  return HERMES_SUCCESS;
}

soter_status_t soter_set_auth_tag_ecb(soter_sym_ctx_t *sym_ctx, const unsigned char * tag, const size_t tag_length){
  return HERMES_NOT_SUPPORTED;
}

soter_status_t soter_set_auth_tag_ctr(soter_sym_ctx_t *sym_ctx, const unsigned char * tag, const size_t tag_length){
  return HERMES_NOT_SUPPORTED;
}

soter_status_t soter_get_auth_tag_gcm(soter_sym_ctx_t *sym_ctx, unsigned char * tag, size_t* tag_length){
  if((*tag_length)<SOTER_SYM_AUTHTAG_LENGTH_gcm){
    (*tag_length)=SOTER_SYM_AUTHTAG_LENGTH_gcm;
    return HERMES_BUFFER_TOO_SMALL;
  }
  if(EVP_CIPHER_CTX_ctrl(&(sym_ctx->evp_sym_ctx), EVP_CTRL_GCM_GET_TAG, SOTER_SYM_AUTHTAG_LENGTH_gcm, tag)!=1){
    return HERMES_FAIL;
  }
  (*tag_length)=SOTER_SYM_AUTHTAG_LENGTH_gcm;
  return HERMES_SUCCESS;
}

soter_status_t soter_get_auth_tag_ecb(soter_sym_ctx_t *sym_ctx, unsigned char * tag, size_t* tag_length){
  return HERMES_NOT_SUPPORTED;
}

soter_status_t soter_get_auth_tag_ctr(soter_sym_ctx_t *sym_ctx, const unsigned char * tag, size_t* tag_length){
  return HERMES_NOT_SUPPORTED;
}

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)		\
  size_t soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_key_length(){return 256/8;} \
  size_t soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_block_size(){return 16;} \
  soter_status_t soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_derive_key(const uint8_t* key_material, const size_t key_material_length,  const uint8_t* salt, const size_t salt_length, uint8_t* key, size_t* key_length) \
  {									\
   if(key==NULL || (*key_length)<soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_key_length()){ \
        (*key_length)=soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_key_length(); \
        return HERMES_BUFFER_TOO_SMALL;						\
    }									\
   return soter_##kdf(key_material, key_material_length, salt, salt_length, key, key_length); \
  }									\
  soter_status_t   soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_init(soter_sym_ctx_t *sym_ctx, const uint8_t *key, const uint8_t *iv)\
  {									\
    const uint8_t default_iv[12]={0,0,0,0,0,0,0,0,0,0,0,0};		\
    if(!EVP_##e_or_d##Init_ex(&(sym_ctx->evp_sym_ctx), EVP_##alg##_256_##mode(), NULL, key, (iv==NULL)?default_iv:iv)){ \
      return HERMES_FAIL;						\
    }									\
    return HERMES_SUCCESS;						\
  }									\
  soter_status_t soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_update(soter_sym_ctx_t *sym_ctx, const uint8_t *in, const size_t in_length, uint8_t *out, size_t *out_length) \
  {									\
   if((*out_length)<(in_length+soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_block_size()-1)){ \
      (*out_length)=in_length+soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_block_size()-1; \
      return HERMES_BUFFER_TOO_SMALL;					\
     }									\
   if(!EVP_##e_or_d##Update(&(sym_ctx->evp_sym_ctx), out, (int*)out_length, in, in_length)){ \
     return HERMES_FAIL;						\
   }									\
   return HERMES_SUCCESS;						\
  }									\
  soter_status_t soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_final(soter_sym_ctx_t *sym_ctx, unsigned char *out, size_t *out_length) \
  {									\
    EVP_##e_or_d##Final(&(sym_ctx->evp_sym_ctx), out, (int*)out_length);	\
    return HERMES_SUCCESS;						\
  }		                                                        \
  soter_status_t soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_set_auth_tag(soter_sym_ctx_t *sym_ctx, const unsigned char * tag, const size_t tag_length) \
  {                                                                     \
    return soter_set_auth_tag_##mode(sym_ctx, tag, tag_length);         \
  }                                                                     \
  soter_status_t soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_get_auth_tag(soter_sym_ctx_t *sym_ctx, unsigned char * tag, size_t* tag_length) \
  {                                                                     \
    return soter_get_auth_tag_##mode(sym_ctx, tag, tag_length);         \
  }                                                                     
  

#define SOTER_SYM_ALG(alg, mode, padding, kdf)	\
  SOTER_SYM_ALG_IMPL(alg,mode,padding, kdf, Encrypt)	\
  SOTER_SYM_ALG_IMPL(alg,mode,padding, kdf, Decrypt)

SOTER_SYM_ALGS

#undef SOTER_SYM_ALG_IMPL

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)	\
  case SOTER_##alg##_##mode##_##padding##_##kdf##_##e_or_d:	\
       key_=malloc(soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_key_length()); \
       key_length_=soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_get_key_length(); \
       if(!key_){							\
	 free(ctx);							\
         return NULL;					\
       }								\
       if(soter_##alg##_##mode##_##padding##_##kdf##_##e_or_d##_derive_key(key, key_length, salt, salt_length, key_, &key_length_)!=HERMES_SUCCESS){ \
	 free(ctx);							\
	 return NULL;							\
       }								\
       if(soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_init(ctx, key_, NULL)!=HERMES_SUCCESS){ \
	 free(ctx);							\
	   return NULL;							\
       }								\
      break;

soter_sym_ctx_t* soter_sym_create(const soter_sym_alg_t algId, const void* key, const size_t key_length, const void* salt, const size_t salt_length)
{
  soter_sym_ctx_t* ctx=NULL;
  ctx=malloc(sizeof(soter_sym_ctx_t));
  if(!ctx){
    return NULL;
  }
  uint8_t* key_=NULL;
  size_t key_length_=0;
  EVP_CIPHER_CTX_init(&(ctx->evp_sym_ctx));
  switch(algId){
    SOTER_SYM_ALGS
    default:
      free(key_);
      free(ctx);
      return NULL;
  }
  free(key_);
  ctx->algId=algId;
  return ctx;
}

#undef SOTER_SYM_ALG_IMPL

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)	\
  case SOTER_##alg##_##mode##_##padding##_##kdf##_##e_or_d:	\
    return soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_update(ctx, plain_data, data_length, cipher_data, cipher_data_length);

soter_status_t soter_sym_update(soter_sym_ctx_t *ctx, const void* plain_data, const size_t data_length, void* cipher_data, size_t* cipher_data_length)
{
  switch(ctx->algId){
    SOTER_SYM_ALGS
  }
}

#undef SOTER_SYM_ALG_IMPL

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)	\
  case SOTER_##alg##_##mode##_##padding##_##kdf##_##e_or_d:	\
    return soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_final(ctx, cipher_data, cipher_data_length);
 
soter_status_t soter_sym_final(soter_sym_ctx_t *ctx, void* cipher_data, size_t* cipher_data_length)
{
  switch(ctx->algId){
    SOTER_SYM_ALGS
  }
}

#undef SOTER_SYM_ALG_IMPL

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)	\
  case SOTER_##alg##_##mode##_##padding##_##kdf##_##e_or_d:	\
    return soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_get_auth_tag(ctx, tag, tag_length);
 
soter_status_t soter_sym_get_auth_tag(soter_sym_ctx_t *ctx, void* tag, size_t* tag_length)
{
  switch(ctx->algId){
    SOTER_SYM_ALGS
  }
}

#undef SOTER_SYM_ALG_IMPL

#define SOTER_SYM_ALG_IMPL(alg, mode, padding, kdf, e_or_d)	\
  case SOTER_##alg##_##mode##_##padding##_##kdf##_##e_or_d:	\
    return soter_##alg##_##mode##_##padding##_##kdf##_chipher_##e_or_d##_set_auth_tag(ctx, tag, tag_length);
 
soter_status_t soter_sym_set_auth_tag(soter_sym_ctx_t *ctx, const void* tag, const size_t tag_length)
{
  switch(ctx->algId){
    SOTER_SYM_ALGS
  }
}

#undef SOTER_SYM_ALG_IMPL
 
soter_status_t soter_sym_destroy(soter_sym_ctx_t *ctx)
{
  if(!EVP_CIPHER_CTX_cleanup(&(ctx->evp_sym_ctx)))
    return HERMES_FAIL;
  return HERMES_SUCCESS;
}
