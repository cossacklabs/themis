/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_HMAC_H
#define SOTER_HMAC_H

#include <soter/soter_hash.h>

typedef struct soter_hmac_ctx_type soter_hmac_ctx_t;

soter_hmac_ctx_t* soter_hmac_create(soter_hash_algo_t algo, const uint8_t* key, size_t key_length);
soter_status_t soter_hmac_destroy(soter_hmac_ctx_t *hmac_ctx);
soter_status_t soter_hmac_update(soter_hmac_ctx_t *hmac_ctx, const void *data, size_t length);
soter_status_t soter_hmac_final(soter_hmac_ctx_t *hmac_ctx, uint8_t* hmac_value, size_t* hmac_length);

#endif /* SOTER_HMAC_H */
