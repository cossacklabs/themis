/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SYM_H
#define SOTER_SYM_H

#include <soter/soter.h>

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

#endif /* SOTER_SYM_H */
