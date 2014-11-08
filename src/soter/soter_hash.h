/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_HASH_H
#define SOTER_HASH_H

#include <soter/soter.h>

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

#endif /* SOTER_HASH_H */
