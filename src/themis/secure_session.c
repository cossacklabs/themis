/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/themis.h>
#include <common/error.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>

#include <string.h>

struct data_buf_type
{
	const uint8_t *data;
	size_t length;
};

typedef struct data_buf_type data_buf_t;

static soter_sign_alg_t get_key_sign_type(const void *sign_key, size_t sign_key_length)
{
	const soter_container_hdr_t *key = sign_key;

	if (sign_key_length >= sizeof(soter_container_hdr_t))
	{
		if (sign_key_length < ntohl(key->size))
		{
			(soter_sign_alg_t)0xffffffff;
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

static soter_sign_alg_t get_peer_key_sign_type(const void *sign_key, size_t sign_key_length)
{
	const soter_container_hdr_t *key = sign_key;

	if (sign_key_length >= sizeof(soter_container_hdr_t))
	{
		if (sign_key_length < ntohl(key->size))
		{
			(soter_sign_alg_t)0xffffffff;
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

static themis_status_t compute_signature(const void *sign_key, size_t sign_key_length, const data_buf_t *sign_data, size_t sign_data_count, void *signature, size_t *signature_length)
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

static themis_status_t verify_signature(const void *verify_key, size_t verify_key_length, const data_buf_t *sign_data, size_t sign_data_count, const void *signature, size_t signature_length)
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

void themis_test_func(void)
{
	uint8_t sign_key[1024];
	size_t sign_key_length = sizeof(sign_key);

	uint8_t verify_key[1024];
	size_t verify_key_length = sizeof(verify_key);

	soter_sign_ctx_t sign_ctx;
	soter_status_t status;

	uint8_t sign_data[123];

	data_buf_t data = {sign_data, sizeof(sign_data)};

	uint8_t sig[512];
	size_t sig_len = sizeof(sig);

	status = soter_sign_init(&sign_ctx, SOTER_SIGN_ecdsa_none_pkcs8, NULL, 0, NULL, 0);
	if (status)
	{
		printf("soter_sign_init: %d\n", status);
		return;
	}

	status = soter_sign_export_key(&sign_ctx, sign_key, &sign_key_length, true);
	if (status)
	{
		printf("soter_sign_init: %d\n", status);
		return;
	}

	status = soter_sign_export_key(&sign_ctx, verify_key, &verify_key_length, false);
	if (status)
	{
		printf("soter_sign_init: %d\n", status);
		return;
	}
	printf("%d: %d %d\n", __LINE__, (int)sign_key_length, (int)verify_key_length);

	status = compute_signature(sign_key, sign_key_length, &data, 1, sig, &sig_len);
	printf("%d: %d %d\n", __LINE__, status, (int)sig_len);

	status = verify_signature(verify_key, verify_key_length, &data, 1, sig, sig_len);
	printf("%d: %d %d\n", __LINE__, status, (int)sig_len);

	sign_data[2] ^= 0xff;

	status = verify_signature(verify_key, verify_key_length, &data, 1, sig, sig_len);
	printf("%d: %d %d\n", __LINE__, status, (int)sig_len);

}

themis_status_t secure_session_cleanup(secure_session_t *session_ctx)
{
	if (NULL == session_ctx)
	{
		return HERMES_INVALID_PARAMETER;
	}

	secure_session_peer_cleanup(&(session_ctx->peer));
	secure_session_peer_cleanup(&(session_ctx->we));

	soter_asym_ka_cleanup(&(session_ctx->ecdh_ctx));
	memset(session_ctx, 0, sizeof(secure_session_t));

	return HERMES_SUCCESS;
}

themis_status_t secure_session_init(secure_session_t *session_ctx, const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks)
{
	soter_status_t soter_status;
	themis_status_t res = HERMES_SUCCESS;

	/* TODO: validate input parameters including callback pointers */
	res = secure_session_peer_init(&(session_ctx->we), id, id_length, NULL, 0, sign_key, sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	session_ctx->user_callbacks = user_callbacks;

	soter_status = soter_asym_ka_init(&(session_ctx->ecdh_ctx), SOTER_ASYM_KA_EC_P256);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	soter_status = soter_asym_ka_gen_key(&(session_ctx->ecdh_ctx));
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

err:

	if (HERMES_SUCCESS != res)
	{
		secure_session_cleanup(session_ctx);
	}

	return res;
}

themis_status_t secure_session_connect(secure_session_t *session_ctx)
{
	uint8_t *data_to_send = NULL;
	size_t length_to_send;

	size_t ecdh_key_length = 0;
	size_t signature_length = 0;

	soter_status_t soter_status;
	themis_status_t res = HERMES_SUCCESS;

	data_buf_t sign_data;

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), NULL, &ecdh_key_length, false);
	if (HERMES_BUFFER_TOO_SMALL != soter_status)
	{
		res = soter_status;
		goto err;
	}

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, NULL, 0, NULL, &signature_length);
	if (HERMES_BUFFER_TOO_SMALL != res)
	{
		goto err;
	}

	length_to_send = session_ctx->we.id_length + ecdh_key_length + signature_length;
	data_to_send = malloc(length_to_send);
	if (NULL == data_to_send)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	memcpy(data_to_send, session_ctx->we.id, session_ctx->we.id_length);

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), data_to_send + session_ctx->we.id_length, &ecdh_key_length, false);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	sign_data.data = data_to_send + session_ctx->we.id_length;
	sign_data.length = ecdh_key_length;

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, &sign_data, 1, data_to_send + session_ctx->we.id_length + ecdh_key_length, &signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	session_ctx->user_callbacks->send_data(data_to_send, length_to_send, session_ctx->user_callbacks->user_data);

err:

	if (data_to_send)
	{
		free(data_to_send);
	}

	return res;
}

themis_status_t secure_session_accept(secure_session_t *session_ctx, const void *data, size_t data_length)
{
	const uint8_t *peer_id = data;
	const soter_container_hdr_t *peer_ecdh_key = (const soter_container_hdr_t *)(peer_id + session_ctx->we.id_length);
	size_t peer_ecdh_key_length;

	const uint8_t *signature;
	size_t signature_length;

	themis_status_t res = HERMES_SUCCESS;
	soter_status_t soter_status;

	uint8_t sign_key[1024]; /* Should be enough for RSA 8192 which is 512 bytes */
	size_t sign_key_length;

	const soter_container_hdr_t *peer_sign_key;
	data_buf_t sign_data;

	/* TODO: This code assumes that ids are same length within the system */
	if (data_length < (session_ctx->we.id_length + sizeof(soter_container_hdr_t)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (memcmp(peer_ecdh_key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_ecdh_key_length = ntohl(peer_ecdh_key->size);

	if (data_length < (session_ctx->we.id_length + peer_ecdh_key_length))
	{
		return HERMES_INVALID_PARAMETER;
	}

	signature = peer_id + data_length + peer_ecdh_key_length;
	signature_length = peer_id + data_length - signature;

	if (session_ctx->user_callbacks->get_public_key_for_id(peer_id, session_ctx->we.id_length, sign_key, sizeof(sign_key), session_ctx->user_callbacks->user_data))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	peer_sign_key = (const soter_container_hdr_t *)sign_key;

	if (memcmp(peer_sign_key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	sign_key_length = ntohl(peer_sign_key->size);

	if (sizeof(soter_container_hdr_t) >= sign_key_length)
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	sign_data.data = (const uint8_t *)peer_ecdh_key;
	sign_data.length = peer_ecdh_key_length;

	res = verify_signature(peer_sign_key, sign_key_length, &sign_data, 1, signature, signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	res = secure_session_peer_init(&(session_ctx->peer), peer_id, session_ctx->we.id_length, peer_ecdh_key, peer_ecdh_key_length, peer_sign_key, sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

err:

	if (HERMES_SUCCESS != res)
	{
		secure_session_peer_cleanup(&(session_ctx->peer));
	}

	return res;
}
