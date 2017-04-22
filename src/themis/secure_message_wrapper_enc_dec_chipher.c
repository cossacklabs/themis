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

#include <string.h>


#include <themis/error.h>
#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_asym_ka.h>
#include <themis/secure_message_wrapper.h>
#include <themis/secure_message_wrapper_enc_dec_chipher.h>


#define THEMIS_SYMM_PASSWD_LENGTH 70
#define THEMIS_SYMM_SALT_LENGTH 16

typedef struct symm_init_ctx_type{
  uint8_t passwd[THEMIS_SYMM_PASSWD_LENGTH];
  uint8_t salt[THEMIS_SYMM_SALT_LENGTH];
} symm_init_ctx_t;

struct themis_secure_message_chipher_encrypt_worker_type{
  soter_asym_cipher_t* asym_cipher;
};

themis_status_t themis_secure_message_chipher_encrypter_destroy(themis_secure_message_chipher_encrypter_t* ctx);


themis_secure_message_chipher_encrypter_t* themis_secure_message_chipher_encrypter_init(const uint8_t* peer_public_key, const size_t peer_public_key_length){
  if(!peer_public_key || !peer_public_key_length){
    return NULL;
  }
  themis_secure_message_chipher_encrypter_t* ctx=malloc(sizeof(themis_secure_message_chipher_encrypter_t));
  assert(ctx);
  ctx->asym_cipher=soter_asym_cipher_create(peer_public_key, peer_public_key_length);
  if(!(ctx->asym_cipher)){
    themis_secure_message_chipher_encrypter_destroy(ctx);
    return NULL;
  }
  return ctx;
}

typedef struct themis_secure_chipher_encrypted_message_hdr_type{
    themis_secure_encrypted_message_hdr_t msg;
    uint32_t encrypted_passwd_length;
} themis_secure_chipher_encrypted_message_hdr_t;

themis_status_t themis_secure_message_chipher_encrypter_proceed(themis_secure_message_chipher_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  size_t symm_passwd_length=0;
  size_t seal_message_length=0;
  if(THEMIS_BUFFER_TOO_SMALL!=soter_asym_cipher_encrypt(ctx->asym_cipher, (const uint8_t*)"123", 3, NULL, &symm_passwd_length)){
    return THEMIS_FAIL;
  }
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_seal((const uint8_t*)"123", 3, NULL, 0, message, message_length, NULL, &seal_message_length)){
    return THEMIS_FAIL;
  }
  if(wrapped_message==NULL || (*wrapped_message_length)<(sizeof(themis_secure_chipher_encrypted_message_hdr_t)+symm_passwd_length+seal_message_length)){
    (*wrapped_message_length)=(sizeof(themis_secure_chipher_encrypted_message_hdr_t)+symm_passwd_length+seal_message_length);
    return THEMIS_BUFFER_TOO_SMALL;
  }
  uint8_t symm_passwd[THEMIS_SYMM_PASSWD_LENGTH];
  if(THEMIS_SUCCESS!=soter_rand(symm_passwd, sizeof(symm_passwd))){
    return THEMIS_FAIL;
  }
  uint8_t* encrypted_symm_pass=wrapped_message+sizeof(themis_secure_chipher_encrypted_message_hdr_t);
  size_t encrypted_symm_pass_length=symm_passwd_length;
  if(THEMIS_SUCCESS!=soter_asym_cipher_encrypt(ctx->asym_cipher, symm_passwd, sizeof(symm_passwd), encrypted_symm_pass, &encrypted_symm_pass_length)){
    return THEMIS_FAIL;
  }
  (((themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length)=(uint32_t)encrypted_symm_pass_length;
  uint8_t* encrypted_message=encrypted_symm_pass+encrypted_symm_pass_length;
  size_t encrypted_message_length=seal_message_length;
  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_seal(symm_passwd, sizeof(symm_passwd), NULL, 0, message, message_length, encrypted_message, &encrypted_message_length)){
    return THEMIS_FAIL;
  }
  (*wrapped_message_length)=sizeof(themis_secure_chipher_encrypted_message_hdr_t)+encrypted_symm_pass_length+encrypted_message_length;
  ((themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_type=THEMIS_SECURE_MESSAGE_CHIPHER_ENCRYPTED;
  ((themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_length=(uint32_t)(*wrapped_message_length);
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_chipher_encrypter_destroy(themis_secure_message_chipher_encrypter_t* ctx){
  if(!ctx){
    return THEMIS_INVALID_PARAMETER;
  }
  if(ctx->asym_cipher!=NULL){
    soter_asym_cipher_destroy(ctx->asym_cipher);
  }
  free(ctx);
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_chipher_decrypter_destroy(themis_secure_message_chipher_decrypter_t* ctx);

themis_secure_message_chipher_decrypter_t* themis_secure_message_chipher_decrypter_init(const uint8_t* private_key, const size_t private_key_length){
  if(!private_key || !private_key_length){
    return NULL;
  }
  themis_secure_message_chipher_decrypter_t* ctx=malloc(sizeof(themis_secure_message_chipher_decrypter_t));
  assert(ctx);
  ctx->asym_cipher=soter_asym_cipher_create(private_key, private_key_length);
  THEMIS_IF_FAIL_(ctx->asym_cipher!=NULL, themis_secure_message_chipher_encrypter_destroy(ctx));
  return ctx;
}

themis_status_t themis_secure_message_chipher_decrypter_proceed(themis_secure_message_chipher_decrypter_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  if(wrapped_message_length<sizeof(themis_secure_chipher_encrypted_message_hdr_t) || THEMIS_SECURE_MESSAGE_CHIPHER_ENCRYPTED!=(((const themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_type) || wrapped_message_length!=(((const themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_length)){
    return THEMIS_INVALID_PARAMETER;
  }
  size_t ml=0;
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_decrypt_seal((const uint8_t*)"123",3,NULL,0,wrapped_message+sizeof(themis_secure_chipher_encrypted_message_hdr_t)+((const themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length, wrapped_message_length-sizeof(themis_secure_chipher_encrypted_message_hdr_t)-((const themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length, NULL, &ml)){
    return THEMIS_FAIL;
  }
  if((message==NULL)||((*message_length)<ml)){
    (*message_length)=ml;
    return THEMIS_BUFFER_TOO_SMALL;
  }
  uint8_t sym_ctx_buffer[1024];
  size_t sym_ctx_length_=sizeof(sym_ctx_buffer);
  const uint8_t* wrapped_message_=wrapped_message;
  wrapped_message_+=sizeof(themis_secure_chipher_encrypted_message_hdr_t);
  size_t wrapped_message_length_=wrapped_message_length;
  wrapped_message_length_-=sizeof(themis_secure_chipher_encrypted_message_hdr_t);
  if(THEMIS_SUCCESS!=soter_asym_cipher_decrypt(ctx->asym_cipher, wrapped_message_, ((const themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length, sym_ctx_buffer, &sym_ctx_length_)){
    return THEMIS_FAIL;
  }
  wrapped_message_+=((const themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length;
  wrapped_message_length_-=((const themis_secure_chipher_encrypted_message_hdr_t*)wrapped_message)->encrypted_passwd_length;
  if(THEMIS_SUCCESS!=themis_secure_cell_decrypt_seal(sym_ctx_buffer,sym_ctx_length_,NULL,0,wrapped_message_, wrapped_message_length_, message, message_length)){
    return THEMIS_FAIL;
  }
  return THEMIS_SUCCESS;
}

themis_status_t themis_secure_message_chipher_decrypter_destroy(themis_secure_message_chipher_decrypter_t* ctx){
  return themis_secure_message_chipher_encrypter_destroy(ctx);
}
