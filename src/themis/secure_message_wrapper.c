/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>


#include <common/error.h>
#include <soter/soter.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_asym_ka.h>
#include <themis/secure_message_wrapper.h>


#define THEMIS_RSA_SYM_ALG (SOTER_SYM_AES_CTR|SOTER_SYM_256_KEY_LENGTH|SOTER_SYM_PBKDF2)
#define THEMIS_RSA_SYMM_PASSWD_LENGTH 70 //!!! need to aprove
#define THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH 256 //encrypted password for rsa 256 
#define THEMIS_RSA_SYMM_SALT_LENGTH 16

#define THEMIS_EC_SYM_ALG (SOTER_SYM_AES_CTR|SOTER_SYM_256_KEY_LENGTH|SOTER_SYM_PBKDF2)


soter_sign_alg_t get_alg_id(const uint8_t* key, size_t key_length)
{
  if (key_length < sizeof(soter_container_hdr_t) && key_length < ((const soter_container_hdr_t*)key)->size){
    return (soter_sign_alg_t)(-1);
  }
  if (memcmp(((const soter_container_hdr_t*)key)->tag,EC_PRIV_KEY_PREF,3)==0 || memcmp(((const soter_container_hdr_t*)key)->tag,EC_PUB_KEY_PREF,3)==0){
      return SOTER_SIGN_ecdsa_none_pkcs8;
  }
  if (memcmp(((const soter_container_hdr_t*)key)->tag,RSA_PRIV_KEY_PREF,3)==0 || memcmp(((const soter_container_hdr_t*)key)->tag,RSA_PUB_KEY_PREF,3)==0){
      return SOTER_SIGN_rsa_pss_pkcs8;
  }
  return SOTER_SIGN_undefined;
}

themis_secure_message_signer_t* themis_secure_message_signer_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_signer_t* ctx=malloc(sizeof(themis_secure_message_signer_t));
  if(!ctx){
    return NULL;
  }
  ctx->sign_ctx=NULL;
  ctx->sign_ctx=soter_sign_create(get_alg_id(key,key_length),NULL,0,key, key_length);
  if(!(ctx->sign_ctx)){
    free(ctx);
    return NULL;
  }
  return ctx;
}

themis_status_t themis_secure_message_signer_proceed(themis_secure_message_signer_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length)
{
  HERMES_CHECK(ctx!=NULL && ctx->sign_ctx!=NULL);
  HERMES_CHECK(message!=NULL && message_length!=0 && wrapped_message_length!=NULL);
  uint8_t* signature=NULL;
  size_t signature_length=0;
  HERMES_CHECK(soter_sign_update(ctx->sign_ctx, message, message_length)==HERMES_SUCCESS);
  HERMES_CHECK(soter_sign_final(ctx->sign_ctx, signature, &signature_length)==HERMES_BUFFER_TOO_SMALL);
  if((message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t)>(*wrapped_message_length))){
    (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t);
    return HERMES_BUFFER_TOO_SMALL;
  }
  signature=malloc(signature_length);
  HERMES_CHECK(signature!=NULL);
  if(soter_sign_final(ctx->sign_ctx, signature, &signature_length)!=HERMES_SUCCESS){
    free(signature);
    return HERMES_FAIL;
  }
  themis_secure_signed_message_hdr_t hdr;
  switch(soter_sign_get_alg_id(ctx->sign_ctx)){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    hdr.message_hdr.message_type=THEMIS_SECURE_MESSAGE_EC_SIGNED;
    break;
  case SOTER_SIGN_rsa_pss_pkcs8:
    hdr.message_hdr.message_type=THEMIS_SECURE_MESSAGE_RSA_SIGNED;
    break;
  default:
    return HERMES_INVALID_PARAMETER;  
  };
  hdr.message_hdr.message_length=message_length;
  hdr.signature_length=signature_length;
  memcpy(wrapped_message,&hdr,sizeof(themis_secure_signed_message_hdr_t));
  memcpy(wrapped_message+sizeof(themis_secure_signed_message_hdr_t),message,message_length);
  memcpy(wrapped_message+sizeof(themis_secure_signed_message_hdr_t)+message_length,signature,signature_length);
  (*wrapped_message_length)=message_length+signature_length+sizeof(themis_secure_signed_message_hdr_t);
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_signer_destroy(themis_secure_message_signer_t* ctx)
{
  soter_sign_destroy(ctx->sign_ctx);
  ctx->sign_ctx=NULL;
  free(ctx);
  return HERMES_SUCCESS;
}

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length)
{
  themis_secure_message_verifier_t* ctx=malloc(sizeof(themis_secure_message_verifier_t));
  if(!ctx){
    return NULL;
  }
  ctx->verify_ctx=soter_verify_create(get_alg_id(key,key_length),key, key_length, NULL,0);
  if(!ctx->verify_ctx){
    free(ctx);
    return NULL;
  }
  return ctx;
}
 
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length)
{
  HERMES_CHECK(ctx!=NULL);
  HERMES_CHECK(wrapped_message!=NULL && wrapped_message_length!=0 && message_length!=NULL);
  themis_secure_signed_message_hdr_t* msg=(themis_secure_signed_message_hdr_t*)wrapped_message;
  if(((msg->message_hdr.message_type==THEMIS_SECURE_MESSAGE_RSA_SIGNED && soter_verify_get_alg_id(ctx->verify_ctx)!=SOTER_SIGN_rsa_pss_pkcs8)
      || (msg->message_hdr.message_type==THEMIS_SECURE_MESSAGE_EC_SIGNED && soter_verify_get_alg_id(ctx->verify_ctx)!=SOTER_SIGN_ecdsa_none_pkcs8))
     && (msg->message_hdr.message_length+msg->signature_length+sizeof(themis_secure_message_hdr_t)>wrapped_message_length)){
    return HERMES_INVALID_PARAMETER;
  }
  if((*message_length)<msg->message_hdr.message_length){
    (*message_length)=msg->message_hdr.message_length;
    return HERMES_BUFFER_TOO_SMALL;
  }
  HERMES_CHECK(soter_verify_update(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_signed_message_hdr_t), msg->message_hdr.message_length)==HERMES_SUCCESS);
  HERMES_CHECK(soter_verify_final(ctx->verify_ctx, wrapped_message+sizeof(themis_secure_signed_message_hdr_t)+msg->message_hdr.message_length, msg->signature_length)==HERMES_SUCCESS);
  memcpy(message,wrapped_message+sizeof(themis_secure_signed_message_hdr_t),msg->message_hdr.message_length);
  (*message_length)=msg->message_hdr.message_length;
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx){
  soter_verify_destroy(ctx->verify_ctx);
  free(ctx);
  return HERMES_SUCCESS;  
}


/* secure_encryptet_message*/ 
typedef struct symm_init_ctx_type{
  uint8_t passwd[THEMIS_RSA_SYMM_PASSWD_LENGTH];
  uint8_t salt[THEMIS_RSA_SYMM_SALT_LENGTH];
} symm_init_ctx_t;

struct themis_secure_message_rsa_encrypt_worker_type{
  soter_asym_cipher_t* asym_cipher;
};

typedef struct themis_secure_message_rsa_encrypt_worker_type themis_secure_message_rsa_encrypter_t;
themis_status_t themis_secure_message_rsa_encrypter_destroy(themis_secure_message_rsa_encrypter_t* ctx);


themis_secure_message_rsa_encrypter_t* themis_secure_message_rsa_encrypter_init(const uint8_t* peer_public_key, const size_t peer_public_key_length){
  HERMES_CHECK_PARAM_(peer_public_key!=NULL);
  HERMES_CHECK_PARAM_(peer_public_key_length!=0);
  themis_secure_message_rsa_encrypter_t* ctx=malloc(sizeof(themis_secure_message_rsa_encrypter_t));
  HERMES_CHECK_(ctx!=NULL);
  ctx->asym_cipher=soter_asym_cipher_create(SOTER_ASYM_CIPHER_OAEP);
  HERMES_IF_FAIL_(ctx->asym_cipher!=NULL, themis_secure_message_rsa_encrypter_destroy(ctx));
  HERMES_IF_FAIL_(soter_asym_cipher_import_key(ctx->asym_cipher, peer_public_key, peer_public_key_length)==HERMES_SUCCESS, themis_secure_message_rsa_encrypter_destroy(ctx));
  return ctx;
}

themis_status_t themis_secure_message_rsa_encrypter_proceed(themis_secure_message_rsa_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  if((*wrapped_message_length)<(sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH+message_length)){
    (*wrapped_message_length)=(sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH+message_length);
    return HERMES_BUFFER_TOO_SMALL;
  }
  symm_init_ctx_t symm_passwd_salt;
  HERMES_CHECK(soter_rand((uint8_t*)(&symm_passwd_salt), sizeof(symm_passwd_salt))==HERMES_SUCCESS);
  soter_sym_ctx_t* cipher=soter_sym_encrypt_create(THEMIS_RSA_SYM_ALG, symm_passwd_salt.passwd, THEMIS_RSA_SYMM_PASSWD_LENGTH, symm_passwd_salt.salt, THEMIS_RSA_SYMM_SALT_LENGTH, NULL,0);
  HERMES_CHECK(cipher!=NULL);
  size_t symm_passwd_length=THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH;
  uint8_t* curr_wrapped_message=wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t);
  size_t curr_wrapped_message_length=(THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH+message_length);
  HERMES_IF_FAIL(soter_asym_cipher_encrypt(ctx->asym_cipher, (uint8_t*)(&symm_passwd_salt), sizeof(symm_passwd_salt), curr_wrapped_message, &symm_passwd_length)==HERMES_SUCCESS, soter_sym_encrypt_destroy(cipher));
  curr_wrapped_message+=THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH;
  curr_wrapped_message_length-=THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH;  
  HERMES_IF_FAIL(soter_sym_encrypt_update(cipher, message, message_length, curr_wrapped_message, wrapped_message_length)==HERMES_SUCCESS, soter_sym_encrypt_destroy(cipher));
  curr_wrapped_message+=(*wrapped_message_length);
  curr_wrapped_message_length-=(*wrapped_message_length);    
  HERMES_IF_FAIL(soter_sym_encrypt_final(cipher, curr_wrapped_message, &curr_wrapped_message_length)==HERMES_SUCCESS, soter_sym_encrypt_destroy(cipher));
  HERMES_CHECK(soter_sym_encrypt_destroy(cipher)==HERMES_SUCCESS);
  (*wrapped_message_length)+=sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH+curr_wrapped_message_length;
  ((themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_type=THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_CTR;
  ((themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_length=(*wrapped_message_length);  
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_rsa_encrypter_destroy(themis_secure_message_rsa_encrypter_t* ctx){
  HERMES_CHECK_PARAM(ctx!=NULL);
  if(ctx->asym_cipher!=NULL){
    soter_asym_cipher_destroy(ctx->asym_cipher);
  }
  free(ctx);
  return HERMES_SUCCESS;
}

typedef struct themis_secure_message_rsa_encrypt_worker_type themis_secure_message_rsa_decrypter_t;
themis_status_t themis_secure_message_rsa_decrypter_destroy(themis_secure_message_rsa_decrypter_t* ctx);

themis_secure_message_rsa_decrypter_t* themis_secure_message_rsa_decrypter_init(const uint8_t* private_key, const size_t private_key_length){
  HERMES_CHECK_PARAM_(private_key!=NULL);
  HERMES_CHECK_PARAM_(private_key_length!=0);
  themis_secure_message_rsa_decrypter_t* ctx=malloc(sizeof(themis_secure_message_rsa_decrypter_t));
  HERMES_CHECK_(ctx!=NULL);
  ctx->asym_cipher=soter_asym_cipher_create(SOTER_ASYM_CIPHER_OAEP);
  HERMES_IF_FAIL_(ctx->asym_cipher!=NULL, themis_secure_message_rsa_encrypter_destroy(ctx));
  HERMES_IF_FAIL_(soter_asym_cipher_import_key(ctx->asym_cipher, private_key, private_key_length)==HERMES_SUCCESS, themis_secure_message_rsa_decrypter_destroy(ctx));
  return ctx;
}

themis_status_t themis_secure_message_rsa_decrypter_proceed(themis_secure_message_rsa_decrypter_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  HERMES_CHECK_PARAM(wrapped_message_length>sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH);
  HERMES_CHECK_PARAM(((const themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_type==THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_CTR);
  HERMES_CHECK_PARAM(((const themis_secure_encrypted_message_hdr_t*)wrapped_message)->message_hdr.message_length==wrapped_message_length);
  if((*message_length)<(wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH)){
    (*message_length)=(wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t)+THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH);
    return HERMES_BUFFER_TOO_SMALL;
  }
  uint8_t sym_ctx_buffer[THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH];
  size_t sym_ctx_length_=sizeof(sym_ctx_buffer);
  wrapped_message+=sizeof(themis_secure_encrypted_message_hdr_t);
  size_t wrapped_message_length_=wrapped_message_length;
  wrapped_message_length_-=sizeof(themis_secure_encrypted_message_hdr_t);
  HERMES_CHECK(soter_asym_cipher_decrypt(ctx->asym_cipher, wrapped_message, THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH, sym_ctx_buffer, &sym_ctx_length_)==HERMES_SUCCESS && sym_ctx_length_==sizeof(symm_init_ctx_t));
  wrapped_message+=THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH;
  wrapped_message_length_-=THEMIS_RSA_SYMM_ENCRYPTED_PASSWD_LENGTH;
  soter_sym_ctx_t* cipher=soter_sym_decrypt_create( THEMIS_RSA_SYM_ALG, ((symm_init_ctx_t*)sym_ctx_buffer)->passwd, THEMIS_RSA_SYMM_PASSWD_LENGTH, ((symm_init_ctx_t*)sym_ctx_buffer)->salt, THEMIS_RSA_SYMM_SALT_LENGTH, NULL,0);
  HERMES_CHECK(cipher!=NULL);
  HERMES_IF_FAIL(soter_sym_decrypt_update(cipher, wrapped_message, wrapped_message_length_, message, message_length)==HERMES_SUCCESS, soter_sym_decrypt_destroy(cipher));
  size_t pad_length=16;
  HERMES_IF_FAIL(soter_sym_decrypt_final(cipher, message+(*message_length), &pad_length)==HERMES_SUCCESS, soter_sym_decrypt_destroy(cipher));
  (*message_length)+=pad_length;
  HERMES_CHECK(soter_sym_decrypt_destroy(cipher)==HERMES_SUCCESS);
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_rsa_decrypter_destroy(themis_secure_message_rsa_decrypter_t* ctx){
  return themis_secure_message_rsa_encrypter_destroy(ctx);
}


struct themis_secure_message_ec_worker_type{
  uint8_t shared_secret[128];
  size_t shared_secret_length;
};

typedef struct themis_secure_message_ec_worker_type themis_secure_message_ec_t;

themis_status_t themis_secure_message_ec_encrypter_destroy(themis_secure_message_ec_t* ctx);


themis_secure_message_ec_t* themis_secure_message_ec_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  HERMES_CHECK_PARAM_(private_key!=NULL);
  HERMES_CHECK_PARAM_(private_key_length!=0);
  HERMES_CHECK_PARAM_(peer_public_key!=NULL);
  HERMES_CHECK_PARAM_(peer_public_key_length!=0);
  themis_secure_message_ec_t* ctx=malloc(sizeof(themis_secure_message_ec_t));
  HERMES_CHECK_(ctx!=NULL);
  ctx->shared_secret_length=sizeof(ctx->shared_secret);
  soter_asym_ka_t* km=soter_asym_ka_create(SOTER_ASYM_KA_EC_P256);
  HERMES_IF_FAIL_(km!=NULL, themis_secure_message_ec_encrypter_destroy(ctx));
  HERMES_IF_FAIL_(soter_asym_ka_import_key(km, private_key, private_key_length)==HERMES_SUCCESS, (themis_secure_message_ec_encrypter_destroy(ctx), soter_asym_ka_destroy(km)));
  HERMES_IF_FAIL_(soter_asym_ka_derive(km, peer_public_key, peer_public_key_length, ctx->shared_secret, &ctx->shared_secret_length)==HERMES_SUCCESS, (themis_secure_message_ec_encrypter_destroy(ctx), soter_asym_ka_destroy(km)));
  return ctx;
}
themis_status_t themis_secure_message_ec_encrypter_proceed(themis_secure_message_ec_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  HERMES_CHECK_PARAM(ctx!=NULL);
  size_t encrypted_message_length=0;
  HERMES_CHECK(themis_secure_cell_encrypt_full(ctx->shared_secret, ctx->shared_secret_length, message, message_length, NULL, &encrypted_message_length)==HERMES_BUFFER_TOO_SMALL && encrypted_message_length!=0);
  if((*wrapped_message_length)<(sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length)){
    (*wrapped_message_length)=(sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length);
    return HERMES_BUFFER_TOO_SMALL;
  }
  themis_secure_encrypted_message_hdr_t* hdr=(themis_secure_encrypted_message_hdr_t*)wrapped_message;
  hdr->message_hdr.message_type=THEMIS_SECURE_MESSAGE_EC_ENCRYPTED;
  hdr->message_hdr.message_length=sizeof(themis_secure_encrypted_message_hdr_t)+encrypted_message_length;
  encrypted_message_length=(*wrapped_message_length)-sizeof(themis_secure_encrypted_message_hdr_t);
  HERMES_CHECK(themis_secure_cell_encrypt_full(ctx->shared_secret, ctx->shared_secret_length, message, message_length, wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), &encrypted_message_length)==HERMES_SUCCESS);
  (*wrapped_message_length)=encrypted_message_length+sizeof(themis_secure_encrypted_message_hdr_t);
  return HERMES_SUCCESS;
}

themis_status_t themis_secure_message_ec_encrypter_destroy(themis_secure_message_ec_t* ctx){
  HERMES_CHECK_PARAM(ctx!=NULL);
  free(ctx);
  return HERMES_SUCCESS;
}

themis_secure_message_ec_t* themis_secure_message_ec_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  return themis_secure_message_ec_encrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
}
themis_status_t themis_secure_message_ec_decrypter_proceed(themis_secure_message_ec_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  HERMES_CHECK_PARAM(ctx!=NULL);
  HERMES_CHECK_PARAM(wrapped_message_length>sizeof(themis_secure_encrypted_message_hdr_t));
  themis_secure_encrypted_message_hdr_t* hdr=(themis_secure_encrypted_message_hdr_t*)wrapped_message;
  HERMES_CHECK_PARAM(hdr->message_hdr.message_type==THEMIS_SECURE_MESSAGE_EC_ENCRYPTED && wrapped_message_length==hdr->message_hdr.message_length);
  size_t computed_length=0;
  HERMES_CHECK(themis_secure_cell_decrypt_full(ctx->shared_secret, ctx->shared_secret_length, wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t), NULL, &computed_length));
  if((*message_length)<computed_length){
    (*message_length)=computed_length;
    return HERMES_BUFFER_TOO_SMALL;
  }
HERMES_CHECK(themis_secure_cell_decrypt_full(ctx->shared_secret, ctx->shared_secret_length, wrapped_message+sizeof(themis_secure_encrypted_message_hdr_t), wrapped_message_length-sizeof(themis_secure_encrypted_message_hdr_t), message, message_length)==HERMES_SUCCESS);
  return HERMES_SUCCESS;
}
    
themis_status_t themis_secure_message_ec_decrypter_destroy(themis_secure_message_ec_t* ctx){
  return themis_secure_message_ec_encrypter_destroy(ctx);
}


struct themis_secure_message_encrypt_worker_type{
  union CTX{
    themis_secure_message_rsa_encrypter_t* rsa_encrypter;
    themis_secure_message_ec_t* ec_encrypter;
  } ctx;
  soter_sign_alg_t alg;
};

themis_secure_message_encrypter_t* themis_secure_message_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  HERMES_CHECK_(private_key!=NULL && private_key_length!=0);
  HERMES_CHECK_(peer_public_key!=NULL && peer_public_key_length!=0);
  soter_sign_alg_t alg=get_alg_id(private_key, private_key_length);
  HERMES_CHECK_(alg!=SOTER_SIGN_undefined && alg==get_alg_id(peer_public_key,peer_public_key_length));
  themis_secure_message_encrypter_t* ctx=malloc(sizeof(themis_secure_message_encrypter_t));
  HERMES_CHECK_MALLOC_(ctx);
  switch(alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    ctx->ctx.ec_encrypter=themis_secure_message_ec_encrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
    HERMES_CHECK_(ctx);
    ctx->alg=alg;
    return ctx;    
  case SOTER_SIGN_rsa_pss_pkcs8:
    ctx->ctx.rsa_encrypter=themis_secure_message_rsa_encrypter_init(peer_public_key, peer_public_key_length);
    HERMES_CHECK_(ctx);
    ctx->alg=alg;
    return ctx;
  }
  return NULL;
}
themis_status_t themis_secure_message_encrypter_proceed(themis_secure_message_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length){
  HERMES_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_encrypter_proceed(ctx->ctx.ec_encrypter, message, message_length, wrapped_message, wrapped_message_length);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_encrypter_proceed(ctx->ctx.rsa_encrypter, message, message_length, wrapped_message, wrapped_message_length);
  }
  return HERMES_FAIL;
}
themis_status_t themis_secure_message_encrypter_destroy(themis_secure_message_encrypter_t* ctx){
  HERMES_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_encrypter_destroy(ctx->ctx.ec_encrypter);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_encrypter_destroy(ctx->ctx.rsa_encrypter);
  }
  return HERMES_FAIL;  
}


themis_secure_message_decrypter_t* themis_secure_message_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length){
  HERMES_CHECK_(private_key!=NULL && private_key_length!=0);
  HERMES_CHECK_(peer_public_key!=NULL && peer_public_key_length!=0);
  soter_sign_alg_t alg=get_alg_id(private_key, private_key_length);
  HERMES_CHECK_(alg!=SOTER_SIGN_undefined && alg==get_alg_id(peer_public_key,peer_public_key_length));
  themis_secure_message_decrypter_t* ctx=malloc(sizeof(themis_secure_message_decrypter_t));
  HERMES_CHECK_MALLOC_(ctx);
  switch(alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    ctx->ctx.ec_encrypter=themis_secure_message_ec_decrypter_init(private_key, private_key_length, peer_public_key, peer_public_key_length);
    HERMES_CHECK_(ctx);
    ctx->alg=alg;
    return ctx;
  case SOTER_SIGN_rsa_pss_pkcs8:
    ctx->ctx.rsa_encrypter=themis_secure_message_rsa_decrypter_init(private_key, private_key_length);
    HERMES_CHECK_(ctx);
    ctx->alg=alg;
    return ctx;
  }
  return NULL;
}

themis_status_t themis_secure_message_decrypter_proceed(themis_secure_message_decrypter_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length){
  HERMES_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_decrypter_proceed(ctx->ctx.ec_encrypter, wrapped_message, wrapped_message_length, message, message_length);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_decrypter_proceed(ctx->ctx.rsa_encrypter, wrapped_message, wrapped_message_length, message, message_length);
  }
  return HERMES_FAIL;
}

themis_status_t themis_secure_message_decrypter_destroy(themis_secure_message_decrypter_t* ctx){
  HERMES_CHECK(ctx!=NULL);
  switch(ctx->alg){
  case SOTER_SIGN_ecdsa_none_pkcs8:
    return themis_secure_message_ec_decrypter_destroy(ctx->ctx.ec_encrypter);
  case SOTER_SIGN_rsa_pss_pkcs8:
    return themis_secure_message_rsa_decrypter_destroy(ctx->ctx.rsa_encrypter);
  }
  return HERMES_INVALID_PARAMETER;;  
}









