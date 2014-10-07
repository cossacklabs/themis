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

soter_status_t soter_asym_init(soter_asym_ctx_t* asym_ctx, soter_asym_algo_t algo, soter_asym_padding_t pad);
soter_status_t soter_asym_cleanup(soter_asym_ctx_t* asym_ctx);

#endif /* SOTER_T_H */
