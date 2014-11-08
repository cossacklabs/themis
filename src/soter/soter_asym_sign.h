/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_ASYM_SIGN_H
#define SOTER_ASYM_SIGN_H

#include <soter/soter.h>

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

soter_sign_ctx_t* soter_sign_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_sign_update(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);
soter_status_t soter_sign_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_sign_destroy(soter_sign_ctx_t* ctx);

typedef struct soter_sign_ctx_type soter_verify_ctx_t;

soter_verify_ctx_t* soter_verify_create(soter_sign_alg_t alg, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_update(soter_verify_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_verify_final(soter_verify_ctx_t* ctx, void* signature, size_t signature_length);
soter_status_t soter_verify_destroy(soter_verify_ctx_t* ctx);

#endif /* SOTER_ASYM_SIGN_H */
