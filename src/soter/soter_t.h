/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_T_H
#define SOTER_T_H

/* TODO: #ifdef type definition includes when we support different crypto engines */
#include <soter/openssl/soter_openssl.h>

soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, soter_hash_algo_t algo);

soter_status_t soter_asym_cipher_init(soter_asym_cipher_t* asym_cipher, soter_asym_cipher_padding_t pad);
soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher);

#endif /* SOTER_T_H */
