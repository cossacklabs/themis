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

#define THEMIS_SECURE_MESSAGE_RSA_SIGNED           0x2604
#define THEMIS_SECURE_MESSAGE_EC_SIGNED            0x2605
#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED        0x2606
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_ECB 0x2607
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_CBC 0x2608
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_GCM 0x2609
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED_AES_XTS 0x2610

struct themis_secure_message_hdr_type{
  uint32_t message_type;
  uint32_t message_length;
  uint32_t signature_length;
};

typedef struct themis_secure_message_hdr_type themis_secure_message_hdr_t;

struct themis_secure_message_sign_worker_type{
  soter_sign_ctx_t* sign_ctx;
  bool precompute_signature_present;
};

typedef struct themis_secure_message_sign_worker_type themis_secure_message_signer_t;

themis_secure_message_signer_t* themis_secure_message_signer_init(const uint8_t* key, const size_t key_length);
themis_status_t themis_secure_message_sign_wrapper_proceed(themis_secure_message_signer_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t secure_message_singer_destroy(themis_secure_message_signer_t* ctx);

struct themis_secure_message_verify_worker_type{
  soter_verify_ctx_t* verify_ctx;
};
typedef struct themis_secure_message_verify_worker_type themis_secure_message_verifier_t;

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length);
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length);
themis_status_t secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx);

struct themis_secure_message_rsa_encrypt_worker_t
{
  themis_secure_message_signer_t* signer_;
};

#endif /* _THEMIS_SECURE_MESSAGE_WRAPPER_H_ */










