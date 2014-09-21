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
 * @param [in,out] buffer pointer to pointer to a buffer for random bits
 * @param [in] length length of the buffer
 * @return success code
 *
 * This function generates random bits and puts them in memory pointed by buffer pointer. If buffer points to NULL, new memory of size length will be allocated on the heap and the pointer to this memory will be returned using buffer pointer. Otherwise, random bits will be stored in memory pointed by pointer in buffer. It is the responsibility of the caller to free the memory, if it was allocated by this function.
 */
soter_status_t soter_rand(uint8_t** buffer, size_t length);

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
soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t** hash_value, size_t* hash_length);

#endif /* SOTER_H */
