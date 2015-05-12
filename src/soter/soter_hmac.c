/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <soter/soter_t.h>
#include <soter/error.h>

#include <string.h>

static size_t hash_block_size(soter_hash_algo_t algo)
{
	switch (algo)
	{
	case SOTER_HASH_SHA1:
	case SOTER_HASH_SHA256:
		return 64;
	case SOTER_HASH_SHA512:
		return 128;
	default:
		return 0;
	}
}

soter_status_t soter_hmac_init(soter_hmac_ctx_t *hmac_ctx, soter_hash_algo_t algo, const uint8_t* key, size_t key_length)
{
	size_t block_size = hash_block_size(algo);
	uint8_t i_key_pad[HASH_MAX_BLOCK_SIZE];
	soter_status_t res;
	size_t i;
	size_t o_key_pad_length = sizeof(hmac_ctx->o_key_pad);

	if ((NULL == hmac_ctx) || (0 == block_size) || (HASH_MAX_BLOCK_SIZE < block_size) || (NULL == key) || (0 == key_length))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length > block_size)
	{
		res = soter_hash_init(&(hmac_ctx->hash_ctx), algo);
		if (SOTER_SUCCESS != res)
		{
			return res;
		}

		res = soter_hash_update(&(hmac_ctx->hash_ctx), key, key_length);
		if (SOTER_SUCCESS != res)
		{
			return res;
		}

		res = soter_hash_final(&(hmac_ctx->hash_ctx), hmac_ctx->o_key_pad, &o_key_pad_length);
		if (SOTER_SUCCESS != res)
		{
			return res;
		}
	}
	else
	{
		memcpy(hmac_ctx->o_key_pad, key, key_length);
		o_key_pad_length = key_length;
	}

	if (o_key_pad_length < block_size)
	{
		memset(hmac_ctx->o_key_pad + o_key_pad_length, 0, block_size - o_key_pad_length);
	}

	for (i = 0; i < block_size; i++)
	{
		i_key_pad[i] = 0x36 ^ hmac_ctx->o_key_pad[i];
	}

	res = soter_hash_init(&(hmac_ctx->hash_ctx), algo);
	if (SOTER_SUCCESS != res)
	{
		soter_hmac_cleanup(hmac_ctx);
		return res;
	}

	res = soter_hash_update(&(hmac_ctx->hash_ctx), i_key_pad, block_size);
	if (SOTER_SUCCESS != res)
	{
		soter_hmac_cleanup(hmac_ctx);
		return res;
	}

	memset(i_key_pad, 0, sizeof(i_key_pad));

	for (i = 0; i < block_size; i++)
	{
		hmac_ctx->o_key_pad[i] ^= 0x5c;
	}

	hmac_ctx->block_size = block_size;
	hmac_ctx->algo = algo;
	return SOTER_SUCCESS;
}

soter_status_t soter_hmac_cleanup(soter_hmac_ctx_t *hmac_ctx)
{
	if (NULL == hmac_ctx)
	{
		return SOTER_INVALID_PARAMETER;
	}

	memset(hmac_ctx->o_key_pad, 0, sizeof(hmac_ctx->o_key_pad));
	return SOTER_SUCCESS;
}

soter_status_t soter_hmac_update(soter_hmac_ctx_t *hmac_ctx, const void *data, size_t length)
{
	if ((NULL == hmac_ctx) || (NULL == data))
	{
		return SOTER_INVALID_PARAMETER;
	}

	return soter_hash_update(&(hmac_ctx->hash_ctx), data, length);
}

soter_status_t soter_hmac_final(soter_hmac_ctx_t *hmac_ctx, uint8_t* hmac_value, size_t* hmac_length)
{
	soter_status_t res;
	size_t output_length;
	uint8_t i_hash[HASH_MAX_BLOCK_SIZE];

	if ((NULL == hmac_ctx) || (NULL == hmac_length))
	{
		return SOTER_INVALID_PARAMETER;
	}

	res = soter_hash_final(&(hmac_ctx->hash_ctx), NULL, &output_length);
	if (SOTER_BUFFER_TOO_SMALL != res)
	{
		return res;
	}

	if ((NULL == hmac_value) || (output_length > *hmac_length))
	{
		*hmac_length = output_length;
		return SOTER_BUFFER_TOO_SMALL;
	}

	output_length = sizeof(i_hash);

	res = soter_hash_final(&(hmac_ctx->hash_ctx), i_hash, &output_length);
	if (SOTER_SUCCESS != res)
	{
		return res;
	}

	res = soter_hash_init(&(hmac_ctx->hash_ctx), hmac_ctx->algo);
	if (SOTER_SUCCESS != res)
	{
		return res;
	}

	res = soter_hash_update(&(hmac_ctx->hash_ctx), hmac_ctx->o_key_pad, hmac_ctx->block_size);
	if (SOTER_SUCCESS != res)
	{
		return res;
	}

	memset(hmac_ctx->o_key_pad, 0, sizeof(hmac_ctx->o_key_pad));

	res = soter_hash_update(&(hmac_ctx->hash_ctx), i_hash, output_length);
	if (SOTER_SUCCESS != res)
	{
		return res;
	}

	return soter_hash_final(&(hmac_ctx->hash_ctx), hmac_value, hmac_length);
}

soter_hmac_ctx_t* soter_hmac_create(soter_hash_algo_t algo, const uint8_t* key, size_t key_length)
{
	soter_status_t status;
	soter_hmac_ctx_t *ctx = malloc(sizeof(soter_hmac_ctx_t));
	if (!ctx)
	{
		return NULL;
	}

	status = soter_hmac_init(ctx, algo, key, key_length);
	if (SOTER_SUCCESS == status)
	{
		return ctx;
	}
	else
	{
		free(ctx);
		return NULL;
	}
}

soter_status_t soter_hmac_destroy(soter_hmac_ctx_t *hmac_ctx)
{
	soter_status_t res = soter_hmac_cleanup(hmac_ctx);

	if (SOTER_SUCCESS == res)
	{
		free(hmac_ctx);
	}

	return res;
}
