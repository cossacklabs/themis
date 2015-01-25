/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session_utils.h>

#include <soter/soter_t.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>

#include <common/error.h>

#define MAX_HMAC_SIZE 64 /* For HMAC-SHA512 */
#define MIN_VAL(_X_, _Y_) ((_X_ < _Y_) ? (_X_) : (_Y_))

soter_sign_alg_t get_key_sign_type(const void *sign_key, size_t sign_key_length)
{
	const soter_container_hdr_t *key = sign_key;

	if (sign_key_length >= sizeof(soter_container_hdr_t))
	{
		if (sign_key_length < ntohl(key->size))
		{
			return (soter_sign_alg_t)0xffffffff;
		}

		if (!memcmp(key->tag, EC_PRIV_KEY_PREF, strlen(EC_PRIV_KEY_PREF)))
		{
			return SOTER_SIGN_ecdsa_none_pkcs8;
		}

		if (!memcmp(key->tag, RSA_PRIV_KEY_PREF, strlen(RSA_PRIV_KEY_PREF)))
		{
			return SOTER_SIGN_rsa_pss_pkcs8;
		}
	}

	return (soter_sign_alg_t)0xffffffff;
}

soter_sign_alg_t get_peer_key_sign_type(const void *sign_key, size_t sign_key_length)
{
	const soter_container_hdr_t *key = sign_key;

	if (sign_key_length >= sizeof(soter_container_hdr_t))
	{
		if (sign_key_length < ntohl(key->size))
		{
			return (soter_sign_alg_t)0xffffffff;
		}

		if (!memcmp(key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
		{
			return SOTER_SIGN_ecdsa_none_pkcs8;
		}

		if (!memcmp(key->tag, RSA_PUB_KEY_PREF, strlen(RSA_PUB_KEY_PREF)))
		{
			return SOTER_SIGN_rsa_pss_pkcs8;
		}
	}

	return (soter_sign_alg_t)0xffffffff;
}

themis_status_t compute_signature(const void *sign_key, size_t sign_key_length, const data_buf_t *sign_data, size_t sign_data_count, void *signature, size_t *signature_length)
{
	soter_sign_ctx_t sign_ctx;
	soter_status_t soter_status;
	size_t i;

	soter_status = soter_sign_init(&sign_ctx, get_key_sign_type(sign_key, sign_key_length), sign_key, sign_key_length, NULL, 0);
	if (HERMES_SUCCESS != soter_status)
	{
		return soter_status;
	}

	/* This is to compute real signature, not just get output data size */
	if (sign_data && signature)
	{
		for (i = 0; i < sign_data_count; i++)
		{
			soter_status = soter_sign_update(&sign_ctx, sign_data[i].data, sign_data[i].length);
			if (HERMES_SUCCESS != soter_status)
			{
				goto err;
			}
		}
	}

	soter_status = soter_sign_final(&sign_ctx, signature, signature_length);

err:

	/* TODO: cleanup sign ctx */

	return soter_status;
}

themis_status_t verify_signature(const void *verify_key, size_t verify_key_length, const data_buf_t *sign_data, size_t sign_data_count, const void *signature, size_t signature_length)
{
	soter_sign_ctx_t sign_ctx;
	soter_status_t soter_status;
	size_t i;

	soter_status = soter_verify_init(&sign_ctx, get_peer_key_sign_type(verify_key, verify_key_length), NULL, 0, verify_key, verify_key_length);
	if (HERMES_SUCCESS != soter_status)
	{
		return soter_status;
	}

	for (i = 0; i < sign_data_count; i++)
	{
		soter_status = soter_verify_update(&sign_ctx, sign_data[i].data, sign_data[i].length);
		if (HERMES_SUCCESS != soter_status)
		{
			goto err;
		}
	}

	/* TODO: fix verify functions prototypes */
	soter_status = soter_verify_final(&sign_ctx, (void *)signature, signature_length);

err:

	/* TODO: cleanup sign ctx */

	return soter_status;
}

themis_status_t compute_mac(const void *key, size_t key_length, const data_buf_t *data, size_t data_count, void *mac, size_t *mac_length)
{
	soter_hmac_ctx_t mac_ctx;
	soter_status_t soter_status;
	size_t i;

	soter_status = soter_hmac_init(&mac_ctx, SOTER_HASH_SHA256, key, key_length);
	if (HERMES_SUCCESS != soter_status)
	{
		return soter_status;
	}

	/* This is to compute real mac, not just get output data size */
	if (data && mac)
	{
		for (i = 0; i < data_count; i++)
		{
			soter_status = soter_hmac_update(&mac_ctx, data[i].data, data[i].length);
			if (HERMES_SUCCESS != soter_status)
			{
				goto err;
			}
		}
	}

	soter_status = soter_hmac_final(&mac_ctx, mac, mac_length);

err:

	soter_hmac_cleanup(&mac_ctx);

	return soter_status;
}

themis_status_t verify_mac(const void *key, size_t key_length, const data_buf_t *data_buf_t, size_t data_count, const void *mac, size_t mac_length)
{
	uint8_t computed_mac[MAX_HMAC_SIZE];
	size_t computed_mac_length = sizeof(computed_mac);

	themis_status_t res = compute_mac(key, key_length, data_buf_t, data_count, computed_mac, &computed_mac_length);
	if (HERMES_SUCCESS != res)
	{
		return res;
	}

	if (mac_length > computed_mac_length)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (memcmp(mac, computed_mac, mac_length))
	{
		return HERMES_INVALID_SIGNATURE;
	}
	else
	{
		return HERMES_SUCCESS;
	}
}

/* RFC 6189 p 4.5.1 */
themis_status_t themis_kdf(const void *key, size_t key_length, const char *label, const data_buf_t *context, size_t context_count, void *output, size_t output_length)
{
	soter_hmac_ctx_t hmac_ctx;
	soter_status_t soter_status;

	themis_status_t res = HERMES_SUCCESS;
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
	if (HERMES_SUCCESS != soter_status)
	{
		return soter_status;
	}

	/* i (counter) */
	soter_status = soter_hmac_update(&hmac_ctx, out, 4);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* label */
	soter_status = soter_hmac_update(&hmac_ctx, label, strlen(label));
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* 0x00 delimiter */
	soter_status = soter_hmac_update(&hmac_ctx, out, 1);
	if (HERMES_SUCCESS != soter_status)
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
			if (HERMES_SUCCESS != soter_status)
			{
				res = soter_status;
				goto err;
			}
		}
	}

	soter_status = soter_hmac_final(&hmac_ctx, out, &out_length);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	if (output_length > out_length)
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	memcpy(output, out, output_length);

err:

	memset(out, 0, sizeof(out));

	soter_hmac_cleanup(&hmac_ctx);

	return res;
}

themis_status_t encrypt_gcm(const void *key, size_t key_length, const void *iv, size_t iv_length, const void *in, size_t in_length, void *out, size_t out_length)
{
	soter_sym_ctx_t *ctx;
	soter_status_t res;

	size_t bytes_encrypted = out_length;

	if (out_length < (in_length + CIPHER_AUTH_TAG_SIZE))
	{
		return HERMES_BUFFER_TOO_SMALL;
	}

	ctx = soter_sym_aead_encrypt_create(SOTER_SYM_AES_GCM|SOTER_SYM_256_KEY_LENGTH, key, key_length, NULL, 0, iv, iv_length);
	if (NULL == ctx)
	{
		return HERMES_FAIL;
	}

	res = soter_sym_aead_encrypt_update(ctx, in, in_length, out, &bytes_encrypted);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	if (in_length != bytes_encrypted)
	{
		res = HERMES_FAIL;
		goto err;
	}

	bytes_encrypted = out_length - bytes_encrypted;
	out_length = bytes_encrypted;

	res = soter_sym_aead_encrypt_final(ctx, ((uint8_t *)out) + in_length, &out_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	if (CIPHER_AUTH_TAG_SIZE != out_length)
	{
		res = HERMES_FAIL;
		goto err;
	}

err:

	if (NULL != ctx)
	{
		soter_sym_encrypt_destroy(ctx);
	}

	return res;

	/* TODO: we will simulate encryption until we fix gcm */
	/*const uint8_t *input = in;
	uint8_t *output = out;

	size_t i;

	if (out_length < (in_length + CIPHER_AUTH_TAG_SIZE))
	{
		return HERMES_BUFFER_TOO_SMALL;
	}

	for (i = 0; i < in_length; i++)
	{
		output[i] = input[i] ^ 0xff;
	}

	for (i = in_length; i < (in_length + CIPHER_AUTH_TAG_SIZE); i++)
	{
		output[i] = 0xff;
	}

	return HERMES_SUCCESS;*/
}

themis_status_t decrypt_gcm(const void *key, size_t key_length, const void *iv, size_t iv_length, const void *in, size_t in_length, void *out, size_t out_length)
{
	const uint8_t *input = in;
	uint8_t *output = out;

	size_t i;

	if (out_length < (in_length - CIPHER_AUTH_TAG_SIZE))
	{
		return HERMES_BUFFER_TOO_SMALL;
	}

	for (i = 0; i < (in_length - CIPHER_AUTH_TAG_SIZE); i++)
	{
		output[i] = input[i] ^ 0xff;
	}

	for (i = (in_length - CIPHER_AUTH_TAG_SIZE); i < in_length; i++)
	{
		if (0xff != input[i])
		{
			return HERMES_INVALID_SIGNATURE;
		}
	}

	return HERMES_SUCCESS;
}
