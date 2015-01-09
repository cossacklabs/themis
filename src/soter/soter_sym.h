/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SYM_H
#define SOTER_SYM_H

#include <soter/soter.h>

/** soter symetric algorithms enum  */
enum soter_sym_kdf_type
{
    PBKDF2,
    NOKDF
};

typedef enum soter_sym_kdf_type soter_sym_kdf_t;

typedef struct soter_sym_ctx_type soter_sym_ctx_t;

soter_sym_ctx_t* soter_aes_ecb_encrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_ecb_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_ecb_encrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_ecb_encrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_aes_ecb_decrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_ecb_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_ecb_decrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_ecb_decrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_aes_ctr_encrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_ctr_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_ctr_encrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_ctr_encrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_aes_ctr_decrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_ctr_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_ctr_decrypt_final(soter_sym_ctx_t *ctx, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_ctr_decrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_aes_gcm_encrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_gcm_encrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_gcm_encrypt_final(soter_sym_ctx_t *ctx, void* auth_tag, size_t* auth_tag_length,  void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_gcm_encrypt_destroy(soter_sym_ctx_t *ctx);

soter_sym_ctx_t* soter_aes_gcm_decrypt_create(const soter_sym_kdf_t, const void* key, const size_t key_length, const void* salt, const size_t salt_length);
soter_status_t soter_aes_gcm_decrypt_update(soter_sym_ctx_t *ctx, const void* plain_data,  const size_t data_length, void* chiper_data, size_t* chipher_data_length);
soter_status_t soter_aes_gcm_decrypt_final(soter_sym_ctx_t *ctx, const void* auth_tag, const size_t auth_tag_length, void* chipher_data, size_t* chipher_data_length);
soter_status_t soter_aes_gcm_decrypt_destroy(soter_sym_ctx_t *ctx);

#endif /* SOTER_SYM_H */
