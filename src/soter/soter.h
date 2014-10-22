/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_H
#define SOTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Function return codes
 *
 * TODO: Should define detailed error codes. What codes will we use for errors? It would be good to make them consistent with errno.h where applicable.
 */

typedef int soter_status_t;

/**
 * @brief Generates random bits
 *
 * @param [out] buffer pointer to a buffer for random bits
 * @param [in] length length of the buffer
 * @return success code
 *
 * This function generates random bits and puts them in memory pointed by buffer.
 */
soter_status_t soter_rand(uint8_t* buffer, size_t length);

enum soter_hash_algo_type
  {
	SOTER_HASH_SHA1,
	SOTER_HASH_SHA256,
	SOTER_HASH_SHA512
  };

typedef enum soter_hash_algo_type soter_hash_algo_t;

typedef struct soter_hash_ctx_type soter_hash_ctx_t;

soter_hash_ctx_t* soter_hash_create(soter_hash_algo_t algo);
soter_status_t soter_hash_destroy(soter_hash_ctx_t *hash_ctx);
soter_status_t soter_hash_update(soter_hash_ctx_t *hash_ctx, const void *data, size_t length);
soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t* hash_value, size_t* hash_length);

/**
 * @defgroup SOTER_SYM soter symetric algorithms definition
 *
 * @{
 */
/** define symmetric algorithms groups */
#define SOTER_SYM_ALGS					\
  SOTER_SYM_ALG(aes, ecb, pkcs7, pbkdf2)		\
  SOTER_SYM_ALG(aes, ecb, pkcs7, nonkdf)		\
  SOTER_SYM_ALG(aes, ctr, none,  pbkdf2)		\
  SOTER_SYM_ALG(aes, ctr, none,  nonkdf)		\
  SOTER_SYM_ALG(aes, gcm, none,  pbkdf2)		\
  SOTER_SYM_ALG(aes, gcm, none,  nonkdf)	
//  SOTER_SYM_ALG(aes, xts, none,  pbkdf2)	
//  SOTER_SYM_ALG(aes, xts, none,  nonkdf)	

/** define symmetric algorithm */
#define SOTER_SYM_ALG(alg,mode,padding,kdf)	\
  SOTER_##alg##_##mode##_##padding##_##kdf##_Encrypt,	\
  SOTER_##alg##_##mode##_##padding##_##kdf##_Decrypt,

/** soter symetric algorithms enum  */
enum soter_sym_alg_type
  {
    SOTER_SYM_ALGS
  };

#undef SOTER_SYM_ALG

/** @} */

typedef enum soter_sym_alg_type soter_sym_alg_t;

typedef struct soter_sym_ctx_type soter_sym_ctx_t;

soter_sym_ctx_t* soter_sym_create(const soter_sym_alg_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_sym_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_sym_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_sym_destroy(soter_sym_ctx_t *ctx);


enum soter_asym_cipher_padding_type
{
	SOTER_ASYM_CIPHER_NOPAD,
	SOTER_ASYM_CIPHER_OAEP
};

typedef enum soter_asym_cipher_padding_type soter_asym_cipher_padding_t;

typedef struct soter_asym_cipher_type soter_asym_cipher_t;

soter_asym_cipher_t* soter_asym_cipher_create(soter_asym_cipher_padding_t pad);
soter_status_t soter_asym_cipher_gen_key(soter_asym_cipher_t* asym_cipher_ctx);
soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length);
soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length);
soter_status_t soter_asym_cipher_export_key(soter_asym_cipher_t* asym_cipher_ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_asym_cipher_import_key(soter_asym_cipher_t* asym_cipher_ctx, const void* key, size_t key_length);
soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher_ctx);

enum soter_asym_ka_alg_type
{
	SOTER_ASYM_KA_EC_P256
};

typedef enum soter_asym_ka_alg_type soter_asym_ka_alg_t;

typedef struct soter_asym_ka_type soter_asym_ka_t;

soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg);
soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx);
soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length);
soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length);
soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx);

#define SOTER_SIGN_ALGS				\
  SOTER_SIGN_ALG(rsa,   pss,  pkcs8)	\
  SOTER_SIGN_ALG(ecdsa, none, pkcs8)

#define SOTER_SIGN_ALG(alg, padding, kdf)	\
  SOTER_SIGN_##alg##_##padding##_##kdf,

enum soter_sign_alg_type
  {
    SOTER_SIGN_ALGS
  };
#undef SOTER_SIGN_ALG

typedef enum soter_sign_alg_type soter_sign_alg_t;

typedef struct soter_sign_ctx_type soter_sign_ctx_t;

soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg, const void* key, const size_t key_length);
soter_status_t soter_sign_gen_key(soter_sign_ctx_t* ctx);
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length);
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);
soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx);

#endif /* SOTER_H */
