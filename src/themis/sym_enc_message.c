/**
 * @file
 *
 * (c) CossackLabs
 */

#include <common/error.h>
#include <themis/sym_enc_message.h>
#include <soter/soter.h>

#define THEMIS_AUTH_SYM_ALG (SOTER_SYM_AES_GCM|SOTER_SYM_256_KEY_LENGTH)
#define THEMIS_AUTH_SYM_IV_LENGTH 12
#define THEMIS_AUTH_SYM_AAD_LENGTH 32
#define THEMIS_AUTH_SYM_AUTH_TAG_LENGTH 16

#define THEMIS_SYM_IV_LENGTH 16

themis_status_t themis_auth_sym_plain_encrypt(uint32_t alg,
					      const uint8_t* key,
					      const size_t key_length,
					      const uint8_t* iv,
					      const size_t iv_length,
					      const uint8_t* aad,
					      const size_t aad_length,
					      const uint8_t* message,
					      const size_t message_length,
					      uint8_t* encrypted_message,
					      size_t* encrypted_message_length,
					      uint8_t* auth_tag,
					      size_t* auth_tag_length){
  soter_sym_ctx_t *ctx = soter_sym_aead_encrypt_create(alg, key, key_length, NULL,0,iv, iv_length);
  HERMES_CHECK(ctx!=NULL);
  if(aad!=NULL || aad_length!=0){
    HERMES_CHECK_FREE(soter_sym_aead_encrypt_aad(ctx, aad, aad_length)==HERMES_SUCCESS, ctx);
  }      
  HERMES_CHECK_FREE(soter_sym_aead_encrypt_update(ctx, message, message_length, encrypted_message, encrypted_message_length)==HERMES_SUCCESS, ctx);
  HERMES_CHECK_FREE(soter_sym_aead_encrypt_final(ctx, auth_tag, auth_tag_length)==HERMES_SUCCESS, ctx);
  HERMES_CHECK_FREE(soter_sym_aead_encrypt_destroy(ctx)==HERMES_SUCCESS, ctx);
  return HERMES_SUCCESS;
}

themis_status_t themis_auth_sym_plain_decrypt(uint32_t alg,
					      const uint8_t* key,
					      const size_t key_length,
					      const uint8_t* iv,
					      const size_t iv_length,
					      const uint8_t* aad,
					      const size_t aad_length,
					      const uint8_t* encrypted_message,
					      const size_t encrypted_message_length,
					      uint8_t* message,
					      size_t* message_length,
					      const uint8_t* auth_tag,
					      const size_t auth_tag_length){
  soter_sym_ctx_t *ctx = soter_sym_aead_decrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
  HERMES_CHECK(ctx!=NULL)
  if(aad!=NULL || aad_length!=0){
    HERMES_CHECK_FREE(soter_sym_aead_decrypt_aad(ctx, aad, aad_length)==HERMES_SUCCESS, ctx);
  }      
  HERMES_CHECK_FREE(soter_sym_aead_decrypt_update(ctx, encrypted_message, encrypted_message_length, message, message_length)==HERMES_SUCCESS, ctx);
  HERMES_CHECK_FREE(soter_sym_aead_decrypt_final(ctx, auth_tag, auth_tag_length)==HERMES_SUCCESS, ctx);
  HERMES_CHECK_FREE(soter_sym_aead_decrypt_destroy(ctx)==HERMES_SUCCESS, ctx);
  return HERMES_SUCCESS;
}

themis_status_t themis_sym_plain_encrypt(uint32_t alg,
					 const uint8_t* key,
					 const size_t key_length,
					 const uint8_t* iv,
					 const size_t iv_length,
					 const uint8_t* message,
					 const size_t message_length,
					 uint8_t* encrypted_message,
					 size_t* encrypted_message_length){
  soter_sym_ctx_t *ctx = soter_sym_encrypt_create(alg, key, key_length, NULL,0,iv, iv_length);
  HERMES_CHECK(ctx!=NULL);
  size_t add_length=(*encrypted_message_length);
  HERMES_CHECK_FREE(soter_sym_encrypt_update(ctx, message, message_length, encrypted_message, encrypted_message_length)==HERMES_SUCCESS, ctx);
  add_length-=(*encrypted_message_length);
  HERMES_CHECK_FREE(soter_sym_encrypt_final(ctx, encrypted_message+(*encrypted_message_length), &add_length)==HERMES_SUCCESS, ctx);
  (*encrypted_message_length)+=add_length;
  HERMES_CHECK_FREE(soter_sym_encrypt_destroy(ctx)==HERMES_SUCCESS, ctx);
  return HERMES_SUCCESS;
}

themis_status_t themis_sym_plain_decrypt(uint32_t alg,
					      const uint8_t* key,
					      const size_t key_length,
					      const uint8_t* iv,
					      const size_t iv_length,
					      const uint8_t* encrypted_message,
					      const size_t encrypted_message_length,
					      uint8_t* message,
					      size_t* message_length){
  soter_sym_ctx_t *ctx = soter_sym_decrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
  HERMES_CHECK(ctx!=NULL);
  size_t add_length=(*message_length);
  HERMES_CHECK_FREE(soter_sym_decrypt_update(ctx, encrypted_message, encrypted_message_length, message, message_length)==HERMES_SUCCESS, ctx);
  add_length-=(*message_length);
  HERMES_CHECK_FREE(soter_sym_decrypt_final(ctx, message+(*message_length), &add_length)==HERMES_SUCCESS, ctx);
  (*message_length)+=add_length;
  HERMES_CHECK_FREE(soter_sym_decrypt_destroy(ctx)==HERMES_SUCCESS, ctx);
  return HERMES_SUCCESS;
}


typedef struct themis_auth_sym_message_hdr_type{
  uint32_t alg;
  uint32_t iv_length;
  uint32_t aad_length;
  uint32_t auth_tag_length;
  uint32_t message_length;
} themis_auth_sym_message_hdr_t; 

typedef struct themis_sym_message_hdr_type{
  uint32_t alg;
  uint32_t iv_length;
  uint32_t message_length;
} themis_sym_message_hdr_t; 



themis_status_t themis_auth_sym_encrypt(const uint8_t* key,
					const size_t key_length,
					const uint8_t* message,
					const size_t message_length,
					uint8_t* context,
					size_t* context_length,
					uint8_t* encrypted_message,
					size_t* encrypted_message_length){
  if((*encrypted_message_length)<message_length || (*context_length)<(sizeof(themis_auth_sym_message_hdr_t)+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AAD_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH)){
    (*encrypted_message_length)=message_length;
    (*context_length)=(sizeof(themis_auth_sym_message_hdr_t)+message_length+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AAD_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);
    return HERMES_BUFFER_TOO_SMALL;
  }
  (*encrypted_message_length)=message_length;
  (*context_length)=(sizeof(themis_auth_sym_message_hdr_t)+message_length+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AAD_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);
  themis_auth_sym_message_hdr_t* hdr=(themis_auth_sym_message_hdr_t*)context;
  uint8_t* iv=context+sizeof(themis_auth_sym_message_hdr_t);
  uint8_t* aad=iv+THEMIS_AUTH_SYM_IV_LENGTH;
  uint8_t* auth_tag=aad+THEMIS_AUTH_SYM_AAD_LENGTH;
  HERMES_CHECK(soter_rand(iv, THEMIS_AUTH_SYM_IV_LENGTH)==HERMES_SUCCESS);
  HERMES_CHECK(soter_rand(aad,THEMIS_AUTH_SYM_AAD_LENGTH)==HERMES_SUCCESS);
  hdr->alg=THEMIS_AUTH_SYM_ALG;
  hdr->iv_length=THEMIS_AUTH_SYM_IV_LENGTH;
  hdr->aad_length=THEMIS_AUTH_SYM_AAD_LENGTH;
  hdr->auth_tag_length=THEMIS_AUTH_SYM_AUTH_TAG_LENGTH;
  hdr->message_length=message_length;
  HERMES_CHECK(themis_auth_sym_plain_encrypt(THEMIS_AUTH_SYM_ALG, key, key_length, iv, THEMIS_AUTH_SYM_IV_LENGTH, aad, THEMIS_AUTH_SYM_AAD_LENGTH, message, message_length, encrypted_message, (size_t*)(&(hdr->message_length)), auth_tag, (size_t*)(&(hdr->auth_tag_length)))==HERMES_SUCCESS && hdr->auth_tag_length!=THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);  
  return HERMES_SUCCESS;
}

themis_status_t themis_auth_sym_decrypt(const uint8_t* key,
					const size_t key_length,
					const uint8_t* context,
					const size_t context_length,
					const uint8_t* encrypted_message,
					const size_t encrypted_message_length,
					uint8_t* message,
					size_t* message_length){
  HERMES_CHECK_PARAM(context_length>sizeof(themis_auth_sym_message_hdr_t));
  themis_auth_sym_message_hdr_t* hdr=(themis_auth_sym_message_hdr_t*)context;
  HERMES_CHECK_PARAM(encrypted_message_length>=hdr->message_length);
  HERMES_CHECK_PARAM(context_length >= (sizeof(themis_auth_sym_message_hdr_t)+hdr->iv_length+hdr->aad_length+hdr->auth_tag_length));
  if((*message_length)<hdr->message_length){
    (*message_length)=hdr->message_length;
    return HERMES_BUFFER_TOO_SMALL;
  }
  const uint8_t* iv=context+sizeof(themis_auth_sym_message_hdr_t);
  const uint8_t* aad=iv+hdr->iv_length;
  const uint8_t* auth_tag=aad+hdr->aad_length;
  HERMES_CHECK(themis_auth_sym_plain_decrypt(hdr->alg, key, key_length, iv, hdr->iv_length, aad, hdr->aad_length, encrypted_message, hdr->message_length, message, message_length, auth_tag, hdr->auth_tag_length)==HERMES_SUCCESS);
  return HERMES_SUCCESS;
}

