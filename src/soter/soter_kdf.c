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

#include <soter/soter_kdf.h>
#include <soter/soter_t.h>
#include <soter/error.h>

#include <string.h>

#define MAX_HMAC_SIZE 64 /* For HMAC-SHA512 */
#define MIN_VAL(_X_, _Y_) ((_X_ < _Y_) ? (_X_) : (_Y_))

/* RFC 6189 p 4.5.1 */
soter_status_t soter_kdf(const void *key, size_t key_length, const char *label, const soter_kdf_context_buf_t *context, size_t context_count, void *output, size_t output_length)
{
	soter_hmac_ctx_t hmac_ctx;
	soter_status_t soter_status;

	soter_status_t res = SOTER_SUCCESS;
	uint8_t out[MAX_HMAC_SIZE] = {0, 0, 0, 1};
	size_t out_length = sizeof(out);
	size_t i;
	size_t j;

	uint8_t implicit_key[32];

	/* If key is not specified, we will generate it from other information (useful for using this kdf for generating data from non-secret parameters such as session_id) */
	if (!key)
	{
		memset(implicit_key, 0, sizeof(implicit_key));

		memcpy(implicit_key, label, MIN_VAL(sizeof(implicit_key), strlen(label)));

		for (i = 0; i < context_count; i++)
		{
			if (context[i].data)
			{
				for (j = 0; j < MIN_VAL(sizeof(implicit_key), context[i].length); j++)
				{
					implicit_key[j] ^= context[i].data[j];
				}
			}
		}

		key = implicit_key;
		key_length = sizeof(implicit_key);
	}

	soter_status = soter_hmac_init(&hmac_ctx, SOTER_HASH_SHA256, key, key_length);
	if (SOTER_SUCCESS != soter_status)
	{
		return soter_status;
	}

	/* i (counter) */
	soter_status = soter_hmac_update(&hmac_ctx, out, 4);
	if (SOTER_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* label */
	soter_status = soter_hmac_update(&hmac_ctx, label, strlen(label));
	if (SOTER_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* 0x00 delimiter */
	soter_status = soter_hmac_update(&hmac_ctx, out, 1);
	if (SOTER_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* context */
	for (i = 0; i < context_count; i++)
	{
		if (context[i].data)
		{
			soter_status = soter_hmac_update(&hmac_ctx, context[i].data, context[i].length);
			if (SOTER_SUCCESS != soter_status)
			{
				res = soter_status;
				goto err;
			}
		}
	}

	soter_status = soter_hmac_final(&hmac_ctx, out, &out_length);
	if (SOTER_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	if (output_length > out_length)
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	memcpy(output, out, output_length);

err:

	memset(out, 0, sizeof(out));

	soter_hmac_cleanup(&hmac_ctx);

	return res;
}
