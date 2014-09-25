/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_T_H
#define SOTER_T_H

/* TODO: #ifdef type definition includes when we support different crypto engines */
#include "soter/openssl/soter_openssl.h"

soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, soter_hash_algo_t algo);

#endif /* SOTER_T_H */
