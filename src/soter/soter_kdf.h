/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_KDF_H
#define SOTER_KDF_H

#include <soter/soter.h>

struct soter_kdf_context_buf_type
{
	const uint8_t *data;
	size_t length;
};

typedef struct soter_kdf_context_buf_type soter_kdf_context_buf_t;

soter_status_t soter_kdf(const void *key, size_t key_length, const char *label, const soter_kdf_context_buf_t *context, size_t context_count, void *output, size_t output_length);

#endif /* SOTER_KDF_H */
