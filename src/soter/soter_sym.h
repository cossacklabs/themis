/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SYM_H
#define SOTER_SYM_H

#include <soter/soter.h>

/** soter symetric algorithms  */
#define SOTER_SYM_AES_ECB_PKCS7     0x10010000
#define SOTER_SYM_AES_CTR           0x20010000
#define SOTER_SYM_AES_XTS           0x30010000

/** soter AEAD symetric algorithms  */
#define SOTER_SYM_AES_GCM      0x40010000

#define SOTER_SYM_ALG_MASK          0xf00ff000
#define SOTER_SYM_BLOCK_LENGTH_MASK 0x000ff000

/** soter symetric kdf algorithms */
#define SOTER_SYM_NOKDF             0x00000000
#define SOTER_SYM_PBKDF2            0x01000000
#define SOTER_SYM_KDF_MASK          0x0f000000

/** soter symmetric key lengths*/
#define SOTER_SYM_256_KEY_LENGTH    0x00000100
#define SOTER_SYM_192_KEY_LENGTH    0x000000c0
#define SOTER_SYM_128_KEY_LENGTH    0x00000080
#define SOTER_SYM_KEY_LENGTH_MASK   0x00000fff

typedef struct soter_sym_ctx_type soter_sym_ctx_t;

soter_sym_ctx_t* soter_sym_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);
soter_status_t soter_sym_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_sym_encrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_sym_encrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_sym_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);
soter_status_t soter_sym_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_sym_decrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_sym_decrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_sym_aead_encrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);
soter_status_t soter_sym_aead_encrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length);
soter_status_t soter_sym_aead_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_sym_aead_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length);
soter_status_t soter_sym_aead_encrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_sym_aead_decrypt_create(const uint32_t alg, const void* key, const size_t key_length, const void* salt, const size_t salt_length, const void* iv, const size_t iv_length);
soter_status_t soter_sym_aead_decrypt_aad(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length);
soter_status_t soter_sym_aead_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_sym_aead_decrypt_final(soter_sym_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length);
soter_status_t soter_sym_aead_decrypt_destroy(soter_sym_ctx_t *ctx);
#endif /* SOTER_SYM_H */

