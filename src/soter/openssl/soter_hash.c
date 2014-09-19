/**
 * @file
 *
 * (c) CossackLabs
 */

#include <soter_t.h>

static const EVP_MD* soter_algo_to_evp_md(soter_hash_algo_t algo)
{
	switch (algo)
	{
	case SOTER_HASH_SHA1:
		return EVP_sha1();
	case SOTER_HASH_SHA256:
		return EVP_sha256();
	case SOTER_HASH_SHA512:
		return EVP_sha512();
	default:
		return NULL;
	}
}

soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, soter_hash_algo_t algo)
{
	const EVP_MD *md = soter_algo_to_evp_md(algo);

	if (!md)
		return SOTER_INVALID_PARAMETER;

	if (EVP_DigestInit(&(hash_ctx->evp_md_ctx), md))
		return SOTER_SUCCESS;
	else
		return SOTER_FAIL;
}

soter_status_t soter_hash_update(soter_hash_ctx_t *hash_ctx, const void *data, size_t length)
{
	if (EVP_DigestUpdate(&(hash_ctx->evp_md_ctx), data, length))
		return SOTER_SUCCESS;
	else
		return SOTER_FAIL;
}

soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t** hash_value, size_t* hash_length)
{
	size_t md_length;
	uint8_t *bytes;

	if ((!hash_value) || (!hash_length))
		return SOTER_INVALID_PARAMETER;

	bytes = *hash_value;
	md_length = (size_t)EVP_MD_CTX_size(&(hash_ctx->evp_md_ctx));

	if (bytes)
	{
		if (md_length > *hash_length)
			return SOTER_INVALID_PARAMETER; /* TODO: Should we add separate "buffer too small" error? */
	}
	else
	{
		bytes = malloc(md_length);
		if (!bytes)
			return SOTER_NO_MEMORY;
	}

	if (EVP_DigestFinal(&(hash_ctx->evp_md_ctx), bytes, (unsigned int *)&md_length))
	{
		*hash_value = bytes;
		*hash_length = md_length;
		return SOTER_SUCCESS;
	}
	else
	{
		if (!(*hash_value))
			free(bytes);
		return SOTER_FAIL;
	}
}

soter_hash_ctx_t* soter_hash_create(soter_hash_algo_t algo)
{
	soter_status_t status;
	soter_hash_ctx_t *ctx = malloc(sizeof(soter_hash_ctx_t));
	if (!ctx)
		return NULL;

	status = soter_hash_init(ctx, algo);
	if (SOTER_SUCCESS == status)
		return ctx;
	else
	{
		free(ctx);
		return NULL;
	}
}

soter_status_t soter_hash_destroy(soter_hash_ctx_t *hash_ctx)
{
	if (!hash_ctx)
		return SOTER_INVALID_PARAMETER;

	free(hash_ctx);
	return SOTER_SUCCESS;
}
