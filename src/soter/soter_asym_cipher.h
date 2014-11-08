/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_ASYM_CIPHER_H
#define SOTER_ASYM_CIPHER_H

#include <soter/soter.h>

enum soter_asym_cipher_padding_type
{
	SOTER_ASYM_CIPHER_NOPAD,
	SOTER_ASYM_CIPHER_OAEP
};

typedef enum soter_asym_cipher_padding_type soter_asym_cipher_padding_t;

typedef struct soter_asym_cipher_type soter_asym_cipher_t;

soter_asym_cipher_t* soter_asym_cipher_create(soter_asym_cipher_padding_t pad);
soter_status_t soter_asym_cipher_gen_key(soter_asym_cipher_t* asym_cipher_ctx);
soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length);
soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher_ctx, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length);
soter_status_t soter_asym_cipher_export_key(soter_asym_cipher_t* asym_cipher_ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_asym_cipher_import_key(soter_asym_cipher_t* asym_cipher_ctx, const void* key, size_t key_length);
soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher_ctx);

#endif /* SOTER_ASYM_CIPHER_H */
