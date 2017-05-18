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

#ifndef SOTER_ENGINE_H
#define SOTER_ENGINE_H

#include <soter/soter.h>
#include <openssl/evp.h>

struct soter_hash_ctx_type
{
  EVP_MD_CTX evp_md_ctx;
};

struct soter_sym_ctx_type
{
  uint32_t alg;
  EVP_CIPHER_CTX evp_sym_ctx;
};

struct soter_sym_aead_ctx_type
{
  uint32_t alg;
  EVP_CIPHER_CTX evp_sym_ctx;
};

struct soter_asym_cipher_type
{
	EVP_PKEY_CTX *pkey_ctx;
};

struct soter_rsa_key_pair_gen_type{
	EVP_PKEY_CTX *pkey_ctx;
};

struct soter_ec_key_pair_gen_type{
	EVP_PKEY_CTX *pkey_ctx;
};


struct soter_asym_ka_type
{
	EVP_PKEY_CTX *pkey_ctx;
};

struct soter_sign_ctx_type{
  EVP_PKEY_CTX *pkey_ctx;
  EVP_MD_CTX *md_ctx;
  uint32_t alg;
};

#define SOTER_CRYPTER_SEMAPHORE_NAME "soterCrypterSem"
#define SOTER_CRYPTER_MAIN_SHM_NAME "soterCrypterMainSharedMemName"
#define SOTER_CRYPTER_IN_PARAMS_SHM_NAME "soterCrypterInSharedMemName"
#define SOTER_CRYPTER_OUT_PARAMS_SHM_NAME "soterCrypterInSharedMemName"

typedef struct soter_crypter_params_type{
  uint32_t op_code;
  size_t param1_len;
  size_t param2_len;
  size_t param3_len;
}soter_crypter_params_t;

typedef enum {
  ASYM_GEN,
  KA_CREATE,
  KA_DESTROY,
  KA_DERIVE,
  CIPHER_CREATE,
  CIPHER_DESTROY,
  CIPHER_ENCRYPT,
  CIPHER_DECRYPT,
  SIGN_CREATE,
  SIGN_DESTROY,
  SIGN_UPDATE,
  SIGN_FINAL,
  VERIFY_CREATE,
  VERIFY_DESTROY,
  VERIFY_UPDATE,
  VERIFY_FINAL
} soter_crypter_op_code_t;

#endif /* SOTER_ENGINE_H */
