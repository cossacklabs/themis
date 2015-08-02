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

#ifndef _THEMIS_SECURE_MESSAGE_WRAPPER_H_
#define _THEMIS_SECURE_MESSAGE_WRAPPER_H_

#include <themis/themis.h>
#include <soter/soter.h>

#define THEMIS_SECURE_MESSAGE                      0x26040000

#define THEMIS_SECURE_MESSAGE_SIGNED               (THEMIS_SECURE_MESSAGE ^ 0x00002600)
#define THEMIS_SECURE_MESSAGE_RSA_SIGNED           (THEMIS_SECURE_MESSAGE_SIGNED ^ 0x00000010)
#define THEMIS_SECURE_MESSAGE_EC_SIGNED            (THEMIS_SECURE_MESSAGE_SIGNED ^ 0x00000020)

#define IS_THEMIS_SECURE_MESSAGE_SIGNED(tag)       ((tag&0xffffff00)==THEMIS_SECURE_MESSAGE_SIGNED?true:false)      

#define THEMIS_SECURE_MESSAGE_ENCRYPTED             (THEMIS_SECURE_MESSAGE ^ 0x00002700)
#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED         (THEMIS_SECURE_MESSAGE_ENCRYPTED ^ 0x00000010)
//#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_ECB (THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED ^ 0x00000011)
//#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_CTR (THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED ^ 0x00000012)
//#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_GCM (THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED ^ 0x00000013)
//#define THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED_AES_XTS (THEMIS_SECURE_MESSAGE_RSA_ENCRYPTED ^ 0x00000014)
#define THEMIS_SECURE_MESSAGE_EC_ENCRYPTED  (THEMIS_SECURE_MESSAGE_ENCRYPTED ^ 0x00000020)

#define IS_THEMIS_SECURE_MESSAGE_ENCRYPTED(tag)    ((tag&0xffffff00)==THEMIS_SECURE_MESSAGE_ENCRYPTED?true:false)      

struct themis_secure_message_hdr_type{
  uint32_t message_type;
  uint32_t message_length;
};
typedef struct themis_secure_message_hdr_type themis_secure_message_hdr_t;

#define THEMIS_SECURE_MESSAGE_LENGTH(hdr) (hdr->message_length)

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
themis_status_t themis_secure_message_signer_destroy(themis_secure_message_signer_t* ctx);

struct themis_secure_message_verify_worker_type{
  soter_verify_ctx_t* verify_ctx;
};
typedef struct themis_secure_message_verify_worker_type themis_secure_message_verifier_t;

themis_secure_message_verifier_t* themis_secure_message_verifier_init(const uint8_t* key, const size_t key_length);
themis_status_t themis_secure_message_verifier_proceed(themis_secure_message_verifier_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length);
themis_status_t themis_secure_message_verifier_destroy(themis_secure_message_verifier_t* ctx);

struct themis_secure_message_encrypt_worker_type;

typedef struct themis_secure_message_encrypt_worker_type themis_secure_message_encrypter_t; 

themis_secure_message_encrypter_t* themis_secure_message_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_encrypter_proceed(themis_secure_message_encrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t themis_secure_message_encrypter_destroy(themis_secure_message_encrypter_t* ctx);

typedef struct themis_secure_message_encrypt_worker_type themis_secure_message_decrypter_t; 

themis_secure_message_decrypter_t* themis_secure_message_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_decrypter_proceed(themis_secure_message_decrypter_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_status_t themis_secure_message_decrypter_destroy(themis_secure_message_decrypter_t* ctx);



#endif /* _THEMIS_SECURE_MESSAGE_WRAPPER_H_ */










