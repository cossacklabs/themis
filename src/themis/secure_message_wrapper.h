/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _THEMIS_SECURE_MESSAGE_WRAPPER_H_
#define _THEMIS_SECURE_MESSAGE_WRAPPER_H_

#include "common/error.h"
#include "themis/themis.h"
#include "soter/soter.h"

#define THEMIS_SECURE_MESSAGE                      0x26040000

#define THEMIS_SECURE_MESSAGE_SIGNED               (THEMIS_SECURE_MESSAGE ^ 0x00002600)
#define THEMIS_SECURE_MESSAGE_RSA_SIGNED           (THEMIS_SECURE_MESSAGE_SIGNED ^ 0x00000010)
#define THEMIS_SECURE_MESSAGE_EC_SIGNED            (THEMIS_SECURE_MESSAGE_SIGNED ^ 0x00000020)

#define IS_THEMIS_SECURE_MESSAGE_SIGNED(tag)       ((tag&0x00002600)==0x00002600?true:false)      

#define THEMIS_SECIRE_MESSAGE_ENCRYPTED            (THEMIS_SECURE_MESSAGE ^ 0x00002700)
#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED        (THEMIS_SECIRE_MESSAGE_ENCRYPTED ^ 0x00000010)
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_ECB (THEMIS_SECIRE_MESSAGE_ENCRYPTED ^ 0x00000021)
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_CBC (THEMIS_SECIRE_MESSAGE_ENCRYPTED ^ 0x00000022)
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_GCM (THEMIS_SECIRE_MESSAGE_ENCRYPTED ^ 0x00000023)
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_XTS (THEMIS_SECIRE_MESSAGE_ENCRYPTED ^ 0x00000024)

#define IS_THEMIS_SECURE_MESSAGE_ENCLYPTED(tag)    ((tag&0x00002700)==0x00002700?true:false)      

struct themis_secure_message_hdr_type{
  uint32_t message_type;
  uint32_t message_length;
};
typedef struct themis_secure_message_hdr_type themis_secure_message_hdr_t;

#define THEMIS_SECURE_MESSAGE_LENGTH(hdr) (sizeof(themis_secure_message_hdr_t)+hdr->message_length)

struct themis_secure_signed_message_hdr_type{
  themis_secure_message_hdr_t message_hdr;
  uint32_t signature_length;
};

typedef struct themis_secure_signed_message_hdr_type themis_secure_signed_message_hdr_t; 

typedef struct themis_secure_encrypted_message_hdr_type{
  themis_secure_message_hdr_t message_hdr;
} themis_secure_encrypted_message_hdr_t;


struct themis_secure_message_sign_worker_type{
  soter_sign_ctx_t* sign_ctx;
};

typedef struct themis_secure_message_sign_worker_type themis_secure_message_signer_t;

themis_secure_message_signer_t* themis_secure_message_signer_init(const uint8_t* key, const size_t key_length);
themis_status_t themis_secure_message_signer_proceed(themis_secure_message_signer_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_signer_destroy(themis_secure_message_signer_t* ctx);

struct themis_secure_message_verify_worker_type{
  soter_verify_ctx_t* verify_ctx;
};
typedef struct themis_secure_message_verify_worker_type themis_secure_message_verifier_t;

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length);
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length);
themis_status_t secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx);

struct themis_secure_message_rsa_encrypt_worker_type;

typedef struct themis_secure_message_rsa_encrypt_worker_type themis_secure_message_rsa_encrypter_t; 

themis_secure_message_rsa_encrypter_t* themis_secure_message_rsa_encrypter_init(const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_rsa_encrypter_proceed(themis_secure_message_rsa_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_rsa_encrypter_destroy(themis_secure_message_rsa_encrypter_t* ctx);

typedef struct themis_secure_message_rsa_encrypt_worker_type themis_secure_message_rsa_decrypter_t; 

themis_secure_message_rsa_decrypter_t* themis_secure_message_rsa_decrypter_init(const uint8_t* private_key, const size_t private_key_length);
themis_status_t themis_secure_message_rsa_decrypter_proceed(themis_secure_message_rsa_decrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_rsa_decrypter_destroy(themis_secure_message_rsa_decrypter_t* ctx);

struct themis_secure_message_ec_worker_type{
  soter_sym_ctx_t* cipher;
};

typedef struct themis_secure_message_ec_worker_type themis_secure_message_ec_encrypter_t; 

themis_secure_message_ec_encrypter_t* themis_secure_message_ec_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_ec_encrypter_proceed(themis_secure_message_ec_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_ec_encrypter_destroy(themis_secure_message_ec_encrypter_t* ctx);

typedef struct themis_secure_message_ec_worker_type themis_secure_message_ec_decrypter_t; 

themis_secure_message_ec_decrypter_t* themis_secure_message_ec_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_ec_decrypter_proceed(themis_secure_message_ec_decrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_ec_decrypter_destroy(themis_secure_message_ec_decrypter_t* ctx);




#endif /* _THEMIS_SECURE_MESSAGE_WRAPPER_H_ */










