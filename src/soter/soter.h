/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_H
#define SOTER_H

#include <stdint.h>
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


enum soter_sym_alg_type
  {
    SOTER_AES_ECB_PKCS7_PBKDF2_ENCRYPT,
    SOTER_AES_ECB_PKCS7_PBKDF2_DECRYPT,
    SOTER_AES_CTR_PBKDF2_ENCRYPT,
    SOTER_AES_CTR_PBKDF2_DECRYPT,
    SOTER_AES_GCM_PBKDF2_ENCRYPT,
    SOTER_AES_GCM_PBKDF2_DECRYPT,
    SOTER_AES_XTS_PBKDF2_ENCRYPT,
    SOTER_AES_XTS_PBKDF2_DECRYPT    
  };

#define SOTER_AES_KEY_LENGTH 32

typedef enum soter_sym_alg_type soter_sym_alg_t;

typedef struct soter_sym_ctx_type soter_sym_ctx_t;

soter_status_t soter_sym_create(soter_sym_ctx_t *sym_ctx, size_t* sym_ctx_length, const soter_sym_alg_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t sym_update(soter_sym_ctx_t *sym_ctx, const void* plain_data,  const size_t data_length, const void* chiper_data, size_t* chipher_data_length);
soter_status_t sym_final(soter_sym_ctx_t *sym_ctx, const void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_sym_destroy(soter_sym_ctx_t *ctx);


#endif /* SOTER_H */
