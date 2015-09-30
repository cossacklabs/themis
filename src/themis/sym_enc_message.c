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

#include <themis/error.h>
#include <themis/sym_enc_message.h>
#include <soter/soter.h>
#include <string.h>
#include <themis/secure_cell_alg.h>

#define THEMIS_SYM_KDF_KEY_LABEL "Themis secure cell message key"
#define THEMIS_SYM_KDF_IV_LABEL "Themis secure cell message iv"

themis_status_t themis_sym_kdf(const uint8_t* master_key,
			       const size_t master_key_length,
			       const char* label,
			       const uint8_t* context,
			       const size_t context_length,
			       const uint8_t* context2,
			       const size_t context2_length,
			       uint8_t* key,
			       size_t key_length){
  THEMIS_CHECK_PARAM(master_key!=NULL && master_key_length!=0);
  THEMIS_CHECK_PARAM(context!=NULL && context_length!=0);
  soter_kdf_context_buf_t ctx[2]={{context, context_length}, {context2, context2_length}};
  THEMIS_CHECK(soter_kdf(master_key, master_key_length, label, ctx, (context2==NULL||context2_length==0)?1:2, key, key_length)==THEMIS_SUCCESS);
  return THEMIS_SUCCESS;
}

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
  THEMIS_CHECK(ctx!=NULL);
  if(aad!=NULL || aad_length!=0){
    THEMIS_CHECK_FREE(soter_sym_aead_encrypt_aad(ctx, aad, aad_length)==THEMIS_SUCCESS, ctx);
  }      
  THEMIS_CHECK_FREE(soter_sym_aead_encrypt_update(ctx, message, message_length, encrypted_message, encrypted_message_length)==THEMIS_SUCCESS, ctx);
  THEMIS_CHECK_FREE(soter_sym_aead_encrypt_final(ctx, auth_tag, auth_tag_length)==THEMIS_SUCCESS, ctx);
  THEMIS_CHECK_FREE(soter_sym_aead_encrypt_destroy(ctx)==THEMIS_SUCCESS, ctx);
  return THEMIS_SUCCESS;
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
  THEMIS_CHECK(ctx!=NULL)
  if(aad!=NULL || aad_length!=0){
    THEMIS_CHECK_FREE(soter_sym_aead_decrypt_aad(ctx, aad, aad_length)==THEMIS_SUCCESS, ctx);
  }      
  THEMIS_CHECK_FREE(soter_sym_aead_decrypt_update(ctx, encrypted_message, encrypted_message_length, message, message_length)==THEMIS_SUCCESS, ctx);
  THEMIS_CHECK_FREE(soter_sym_aead_decrypt_final(ctx, auth_tag, auth_tag_length)==THEMIS_SUCCESS, ctx);
  THEMIS_CHECK_FREE(soter_sym_aead_decrypt_destroy(ctx)==THEMIS_SUCCESS, ctx);
  return THEMIS_SUCCESS;
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
  THEMIS_CHECK(ctx!=NULL);
  size_t add_length=(*encrypted_message_length);
  THEMIS_STATUS_CHECK_FREE(soter_sym_encrypt_update(ctx, message, message_length, encrypted_message, encrypted_message_length),THEMIS_SUCCESS, ctx);
  add_length-=(*encrypted_message_length);
  THEMIS_CHECK_FREE(soter_sym_encrypt_final(ctx, encrypted_message+(*encrypted_message_length), &add_length)==THEMIS_SUCCESS, ctx);
  (*encrypted_message_length)+=add_length;
  THEMIS_CHECK_FREE(soter_sym_encrypt_destroy(ctx)==THEMIS_SUCCESS, ctx);
  return THEMIS_SUCCESS;
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
  THEMIS_CHECK(ctx!=NULL);
  size_t add_length=(*message_length);
  THEMIS_STATUS_CHECK_FREE(soter_sym_decrypt_update(ctx, encrypted_message, encrypted_message_length, message, message_length),THEMIS_SUCCESS, ctx);
  add_length-=(*message_length);
  THEMIS_CHECK_FREE(soter_sym_decrypt_final(ctx, message+(*message_length), &add_length)==THEMIS_SUCCESS, ctx);
  (*message_length)+=add_length;
  THEMIS_CHECK_FREE(soter_sym_decrypt_destroy(ctx)==THEMIS_SUCCESS, ctx);
  return THEMIS_SUCCESS;
}


typedef struct themis_auth_sym_message_hdr_type{
  uint32_t alg;
  uint32_t iv_length;
  uint32_t auth_tag_length;
  uint32_t message_length;
} themis_auth_sym_message_hdr_t; 

themis_status_t themis_auth_sym_encrypt_message_(const uint8_t* key,
						 const size_t key_length,
						 const uint8_t* message,
						 const size_t message_length,
						 const uint8_t* in_context,
						 const size_t in_context_length,
						 uint8_t* out_context,
						 size_t* out_context_length,
						 uint8_t* encrypted_message,
						 size_t* encrypted_message_length){
  if(encrypted_message==NULL || (*encrypted_message_length)<message_length || out_context==NULL || (*out_context_length)<(sizeof(themis_auth_sym_message_hdr_t)+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH)){
    (*encrypted_message_length)=message_length;
    (*out_context_length)=(sizeof(themis_auth_sym_message_hdr_t)+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);
    return THEMIS_BUFFER_TOO_SMALL;
  }
  (*encrypted_message_length)=message_length;
  (*out_context_length)=(sizeof(themis_auth_sym_message_hdr_t)+THEMIS_AUTH_SYM_IV_LENGTH+THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);
  themis_auth_sym_message_hdr_t* hdr=(themis_auth_sym_message_hdr_t*)out_context;
  uint8_t* iv=out_context+sizeof(themis_auth_sym_message_hdr_t);
  uint8_t* auth_tag=iv+THEMIS_AUTH_SYM_IV_LENGTH;
  THEMIS_CHECK(soter_rand(iv, THEMIS_AUTH_SYM_IV_LENGTH)==THEMIS_SUCCESS);
  hdr->alg=THEMIS_AUTH_SYM_ALG;
  hdr->iv_length=THEMIS_AUTH_SYM_IV_LENGTH;
  hdr->auth_tag_length=THEMIS_AUTH_SYM_AUTH_TAG_LENGTH;
  hdr->message_length=(uint32_t)message_length;
  size_t auth_tag_length=THEMIS_AUTH_SYM_AUTH_TAG_LENGTH;
  THEMIS_CHECK(themis_auth_sym_plain_encrypt(THEMIS_AUTH_SYM_ALG, key, key_length, iv, THEMIS_AUTH_SYM_IV_LENGTH, in_context, in_context_length, message, message_length, encrypted_message, encrypted_message_length, auth_tag, &auth_tag_length)==THEMIS_SUCCESS && auth_tag_length==THEMIS_AUTH_SYM_AUTH_TAG_LENGTH);
  return THEMIS_SUCCESS;
}

themis_status_t themis_auth_sym_encrypt_message(const uint8_t* key,
						 const size_t key_length,
						 const uint8_t* message,
						 const size_t message_length,
						 const uint8_t* in_context,
						 const size_t in_context_length,
						 uint8_t* out_context,
						 size_t* out_context_length,
						 uint8_t* encrypted_message,
						 size_t* encrypted_message_length){
  uint8_t key_[THEMIS_AUTH_SYM_KEY_LENGTH/8];
  THEMIS_CHECK_PARAM(message!=NULL && message_length!=0);
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&message_length), sizeof(message_length), in_context, in_context_length, key_, sizeof(key_)),THEMIS_SUCCESS);
  return themis_auth_sym_encrypt_message_(key_, sizeof(key_), message, message_length, in_context, in_context_length, out_context, out_context_length, encrypted_message, encrypted_message_length);
}
themis_status_t themis_auth_sym_decrypt_message_(const uint8_t* key,
						 const size_t key_length,
						 const uint8_t* in_context,
						 const size_t in_context_length,
						 const uint8_t* context,
						 const size_t context_length,
						 const uint8_t* encrypted_message,
						 const size_t encrypted_message_length,
						 uint8_t* message,
						 size_t* message_length){
  THEMIS_CHECK_PARAM(context_length>sizeof(themis_auth_sym_message_hdr_t));
  themis_auth_sym_message_hdr_t* hdr=(themis_auth_sym_message_hdr_t*)context;
  if(message==NULL || (*message_length)<hdr->message_length){
    (*message_length)=hdr->message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  (*message_length)=hdr->message_length;
  THEMIS_CHECK_PARAM(encrypted_message_length>=hdr->message_length);
  THEMIS_CHECK_PARAM(context_length >= (sizeof(themis_auth_sym_message_hdr_t)+hdr->iv_length+hdr->auth_tag_length));
  const uint8_t* iv=context+sizeof(themis_auth_sym_message_hdr_t);
  const uint8_t* auth_tag=iv+hdr->iv_length;
  THEMIS_CHECK(themis_auth_sym_plain_decrypt(hdr->alg, key, key_length, iv, hdr->iv_length, in_context, in_context_length, encrypted_message, hdr->message_length, message, message_length, auth_tag, hdr->auth_tag_length)==THEMIS_SUCCESS);
  return THEMIS_SUCCESS;
}

themis_status_t themis_auth_sym_decrypt_message(const uint8_t* key,
						const size_t key_length,
						const uint8_t* in_context,
						const size_t in_context_length,
						const uint8_t* context,
						const size_t context_length,
						const uint8_t* encrypted_message,
						const size_t encrypted_message_length,
						uint8_t* message,
						size_t* message_length){
  uint8_t key_[THEMIS_AUTH_SYM_KEY_LENGTH/8];
  THEMIS_CHECK_PARAM(context!=NULL && context_length!=0);
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&encrypted_message_length), sizeof(encrypted_message_length), in_context, in_context_length, key_, sizeof(key_)),THEMIS_SUCCESS);
  return themis_auth_sym_decrypt_message_(key_, sizeof(key_), in_context, in_context_length, context, context_length, encrypted_message, encrypted_message_length, message, message_length);
}


typedef struct themis_sym_message_hdr_type{
  uint32_t alg;
  uint32_t iv_length;
  uint32_t message_length;
} themis_sym_message_hdr_t;

themis_status_t themis_sym_encrypt_message_(const uint8_t* key,
					   const size_t key_length,
					   const uint8_t* message,
					   const size_t message_length,
					    //const uint8_t* in_context,
					    //const size_t in_context_length,
					   uint8_t* out_context,
					   size_t* out_context_length,
					   uint8_t* encrypted_message,
					   size_t* encrypted_message_length){
  if(encrypted_message==NULL || (*encrypted_message_length)<message_length || out_context==NULL  || (*out_context_length)<(sizeof(themis_sym_message_hdr_t)+THEMIS_SYM_IV_LENGTH)){
    (*out_context_length)=(sizeof(themis_sym_message_hdr_t)+THEMIS_SYM_IV_LENGTH);
    (*encrypted_message_length)=message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  (*encrypted_message_length)=message_length;
  (*out_context_length)=(sizeof(themis_sym_message_hdr_t)+THEMIS_SYM_IV_LENGTH);
  themis_sym_message_hdr_t* hdr=(themis_sym_message_hdr_t*)out_context;
  uint8_t* iv=out_context+sizeof(themis_sym_message_hdr_t);
  THEMIS_CHECK(soter_rand(iv, THEMIS_SYM_IV_LENGTH)==THEMIS_SUCCESS);
  hdr->alg=THEMIS_SYM_ALG;
  hdr->iv_length=THEMIS_AUTH_SYM_IV_LENGTH;
  hdr->message_length=(uint32_t)message_length;
  THEMIS_CHECK(themis_sym_plain_encrypt(THEMIS_AUTH_SYM_ALG, key, key_length, iv, THEMIS_SYM_IV_LENGTH, message, message_length, encrypted_message, (size_t*)(&(hdr->message_length)))==THEMIS_SUCCESS);  
  return THEMIS_SUCCESS;
}

themis_status_t themis_sym_encrypt_message(const uint8_t* key,
					   const size_t key_length,
					   const uint8_t* message,
					   const size_t message_length,
					   const uint8_t* in_context,
					   const size_t in_context_length,
					   uint8_t* out_context,
					   size_t* out_context_length,
					   uint8_t* encrypted_message,
					   size_t* encrypted_message_length){
  uint8_t key_[THEMIS_SYM_KEY_LENGTH/8];
  THEMIS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&message_length), sizeof(message_length), in_context, in_context_length, key_, sizeof(key_))==THEMIS_SUCCESS);
  return themis_sym_encrypt_message_(key_, sizeof(key_), message,message_length,/*in_context,in_context_length,*/out_context,out_context_length,encrypted_message,encrypted_message_length);
}

themis_status_t themis_sym_decrypt_message_(const uint8_t* key,
					    const size_t key_length,
					    const uint8_t* context,
					    const size_t context_length,
					    const uint8_t* encrypted_message,
					    const size_t encrypted_message_length,
					    uint8_t* message,
					    size_t* message_length){
  THEMIS_CHECK_PARAM(context_length>sizeof(themis_sym_message_hdr_t));
  themis_sym_message_hdr_t* hdr=(themis_sym_message_hdr_t*)context;
  THEMIS_CHECK_PARAM(encrypted_message_length>=hdr->message_length);
  THEMIS_CHECK_PARAM(context_length >= (sizeof(themis_sym_message_hdr_t)+hdr->iv_length));
  if((*message_length)<hdr->message_length){
    (*message_length)=hdr->message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  const uint8_t* iv=context+sizeof(themis_sym_message_hdr_t);
  THEMIS_CHECK(themis_sym_plain_decrypt(hdr->alg, key, key_length, iv, hdr->iv_length, encrypted_message, hdr->message_length, message, message_length)==THEMIS_SUCCESS);
  return THEMIS_SUCCESS;
}

themis_status_t themis_sym_decrypt_message(const uint8_t* key,
					   const size_t key_length,
					   const uint8_t* in_context,
					   const size_t in_context_length,
					   const uint8_t* context,
					   const size_t context_length,
					   const uint8_t* encrypted_message,
					   const size_t encrypted_message_length,
					   uint8_t* message,
					   size_t* message_length){
  uint8_t key_[THEMIS_SYM_KEY_LENGTH/8];
  THEMIS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&message_length), sizeof(message_length), in_context, in_context_length, key_, sizeof(key_))==THEMIS_SUCCESS);
  return themis_sym_decrypt_message_(key_, sizeof(key_),context,context_length,encrypted_message,encrypted_message_length,message,message_length);
}

themis_status_t themis_sym_encrypt_message_u_(const uint8_t* key,
					     const size_t key_length,
					     const uint8_t* message,
					     const size_t message_length,
					     const uint8_t* context,
					     const size_t context_length,
					     uint8_t* encrypted_message,
					     size_t* encrypted_message_length){
  THEMIS_CHECK_PARAM(message!=NULL && message_length!=0);
  THEMIS_CHECK_PARAM(context!=NULL && context_length!=0);
  if((*encrypted_message_length)<message_length){
    (*encrypted_message_length)=message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  (*encrypted_message_length)=message_length;
  uint8_t iv[THEMIS_SYM_IV_LENGTH];
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_IV_LABEL, context, context_length, NULL, 0, iv, THEMIS_SYM_IV_LENGTH),THEMIS_SUCCESS);
  THEMIS_STATUS_CHECK(themis_sym_plain_encrypt(THEMIS_SYM_ALG, key, key_length, iv, THEMIS_SYM_IV_LENGTH, message, message_length, encrypted_message, encrypted_message_length),THEMIS_SUCCESS);  
  return THEMIS_SUCCESS;
}

themis_status_t themis_sym_encrypt_message_u(const uint8_t* key,
					     const size_t key_length,
					     const uint8_t* context,
					     const size_t context_length,
					     const uint8_t* message,
					     const size_t message_length,
					     uint8_t* encrypted_message,
					     size_t* encrypted_message_length){
  uint8_t key_[THEMIS_SYM_KEY_LENGTH/8];
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&message_length), sizeof(message_length), NULL, 0, key_, sizeof(key_)),THEMIS_SUCCESS);  
  return themis_sym_encrypt_message_u_(key_, sizeof(key_), message,message_length,context,context_length,encrypted_message,encrypted_message_length);
}

themis_status_t themis_sym_decrypt_message_u_(const uint8_t* key,
					   const size_t key_length,
					   const uint8_t* context,
					   const size_t context_length,
					   const uint8_t* encrypted_message,
					   const size_t encrypted_message_length,
					   uint8_t* message,
					   size_t* message_length){
  THEMIS_CHECK_PARAM(encrypted_message!=NULL && encrypted_message_length!=0);
  THEMIS_CHECK_PARAM(context!=NULL && context_length!=0);
  if((*message_length)<encrypted_message_length){
    (*message_length)=encrypted_message_length;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  (*message_length)=encrypted_message_length;
  uint8_t iv[THEMIS_SYM_IV_LENGTH];
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_IV_LABEL, context, context_length, NULL, 0, iv, THEMIS_SYM_IV_LENGTH),THEMIS_SUCCESS);
  THEMIS_STATUS_CHECK(themis_sym_plain_decrypt(THEMIS_SYM_ALG, key, key_length, iv, THEMIS_SYM_IV_LENGTH, encrypted_message, encrypted_message_length, message, message_length),THEMIS_SUCCESS);
  return THEMIS_SUCCESS;
}

themis_status_t themis_sym_decrypt_message_u(const uint8_t* key,
					     const size_t key_length,
					     const uint8_t* context,
					     const size_t context_length,
					     const uint8_t* encrypted_message,
					     const size_t encrypted_message_length,
					     uint8_t* message,
					     size_t* message_length){
  uint8_t key_[THEMIS_SYM_KEY_LENGTH/8];
  THEMIS_STATUS_CHECK(themis_sym_kdf(key,key_length, THEMIS_SYM_KDF_KEY_LABEL, (uint8_t*)(&encrypted_message_length), sizeof(encrypted_message_length), NULL, 0, key_, sizeof(key_)),THEMIS_SUCCESS);
  return themis_sym_decrypt_message_u_(key_,sizeof(key_),context,context_length,encrypted_message,encrypted_message_length,message,message_length);
}
