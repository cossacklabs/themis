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

#ifndef SOTER_T_H
#define SOTER_T_H
#include <soter/soter.h>

#ifdef LIBRESSL
#include <soter/openssl/soter_openssl.h>
#elif OPENSSL
#include <soter/openssl/soter_openssl.h>
#endif

soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, soter_hash_algo_t algo);

soter_status_t soter_asym_cipher_init(soter_asym_cipher_t* asym_cipher, const void* key, const size_t key_length, soter_asym_cipher_padding_t pad);
soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher);

soter_status_t soter_asym_ka_init(soter_asym_ka_t* asym_ka_ctx, soter_asym_ka_alg_t alg);
soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* asym_ka_ctx);

soter_status_t soter_sign_init(soter_sign_ctx_t* ctx, soter_sign_alg_t algId, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_init(soter_sign_ctx_t* ctx, soter_sign_alg_t algId, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);

/* Largest possible block size for supported hash functions (SHA-512) */
#define HASH_MAX_BLOCK_SIZE 128

struct soter_hmac_ctx_type
{
	uint8_t o_key_pad[HASH_MAX_BLOCK_SIZE];
	size_t block_size;
	soter_hash_algo_t algo;
	soter_hash_ctx_t hash_ctx;
};

soter_status_t soter_hmac_init(soter_hmac_ctx_t *hmac_ctx, soter_hash_algo_t algo, const uint8_t* key, size_t key_length);
soter_status_t soter_hmac_cleanup(soter_hmac_ctx_t *hmac_ctx);

#endif /* SOTER_T_H */
