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
	uint8_t *data;
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

		if (!memcmp(key->tag, EC_PRIV_KEY_PREF, sizeof(EC_PRIV_KEY_PREF)))
		{
			return SOTER_SIGN_ecdsa_none_pkcs8;
		}

		if (!memcmp(key->tag, RSA_PRIV_KEY_PREF, sizeof(RSA_PRIV_KEY_PREF)))
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

		if (!memcmp(key->tag, EC_PUB_KEY_PREF, sizeof(EC_PUB_KEY_PREF)))
		{
			return SOTER_SIGN_ecdsa_none_pkcs8;
		}

		if (!memcmp(key->tag, RSA_PUB_KEY_PREF, sizeof(RSA_PUB_KEY_PREF)))
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
	if (signature)
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

themis_status_t secure_session_cleanup(secure_session_t *session_ctx)
{
	if (NULL == session_ctx)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (session_ctx->peer_id)
	{
		free(session_ctx->peer_id);
	}

	if (session_ctx->id)
	{
		free(session_ctx->id);
	}

	soter_asym_ka_cleanup(&(session_ctx->ecdh_ctx));
	memset(session_ctx, 0, sizeof(secure_session_t));

	return HERMES_SUCCESS;
}

themis_status_t secure_session_init(secure_session_t *session_ctx, const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks)
{
	soter_status_t soter_status;
	themis_status_t res = HERMES_SUCCESS;

	/* TODO: validate input parameters including callback pointers */
	session_ctx->id = malloc(id_length + sign_key_length);
	if (NULL == session_ctx->id)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	session_ctx->sign_key = session_ctx->id + id_length;
	session_ctx->id_length = id_length;
	session_ctx->sign_key_length = sign_key_length;
	memcpy(session_ctx->id, id, id_length);
	memcpy(session_ctx->sign_key, sign_key, sign_key_length);

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

	soter_sign_ctx_t sign_ctx;

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), NULL, &ecdh_key_length, false);
	if (HERMES_BUFFER_TOO_SMALL != soter_status)
	{
		res = soter_status;
		goto err;
	}

	/* TODO: Need sign cleanup function. Possible memory leak */
	soter_status = soter_sign_init(&sign_ctx, get_key_sign_type(session_ctx->sign_key, session_ctx->sign_key_length), session_ctx->sign_key, session_ctx->sign_key_length, NULL, 0);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	soter_status = soter_sign_final(&sign_ctx, NULL, &signature_length);
	if (HERMES_BUFFER_TOO_SMALL != soter_status)
	{
		res = soter_status;
		goto err;
	}

	length_to_send = session_ctx->id_length + ecdh_key_length + signature_length;
	data_to_send = malloc(length_to_send);
	if (NULL == data_to_send)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	memcpy(data_to_send, session_ctx->id, session_ctx->id_length);

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), data_to_send + session_ctx->id_length, &ecdh_key_length, false);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	soter_status = soter_sign_update(&sign_ctx, data_to_send + session_ctx->id_length, ecdh_key_length);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	soter_status = soter_sign_final(&sign_ctx, data_to_send + session_ctx->id_length + ecdh_key_length, &signature_length);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	session_ctx->user_callbacks->send_data(data_to_send, length_to_send, session_ctx->user_callbacks->user_data);

err:

	/*TODO: cleanup sign_ctx before return */

	if (data_to_send)
	{
		free(data_to_send);
	}

	return res;
}

themis_status_t secure_session_accept(secure_session_t *session_ctx, const void *data, size_t data_length)
{
	const soter_container_hdr_t *peer_key = (const soter_container_hdr_t *)(((const uint8_t *)data) + session_ctx->id_length);
	themis_status_t res = HERMES_SUCCESS;
	soter_status_t soter_status;

	uint8_t peer_sign_key[1024]; /* Should be enough for RSA 8192 which is 512 bytes */

	soter_sign_ctx_t sign_ctx;

	/* TODO: This code assumes that ids are same length within the system */
	if (data_length < (session_ctx->id_length + sizeof(soter_container_hdr_t)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (memcmp(peer_key->tag, EC_PUB_KEY_PREF, sizeof(EC_PUB_KEY_PREF)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	session_ctx->peer_id = malloc(session_ctx->id_length + ntohls(peer_key->size));
	if (NULL == session_ctx->peer_id)
	{
		return HERMES_NO_MEMORY;
	}

	session_ctx->peer_id_length = session_ctx->id_length;
	session_ctx->peer_key = session_ctx->peer_id + session_ctx->peer_id_length;
	session_ctx->peer_key_length = ntohls(peer_key->size);

	memcpy(session_ctx->peer_id, data, session_ctx->peer_id_length);
	memcpy(session_ctx->peer_key, peer_key, session_ctx->peer_key_length);

	if (session_ctx->user_callbacks->get_public_key_for_id(session_ctx->peer_id, session_ctx->peer_id_length, peer_sign_key, sizeof(peer_sign_key), session_ctx->user_callbacks->user_data))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	peer_key = (const soter_container_hdr_t *)peer_sign_key;

	//if (memcmp(peer_key->tag, EC_PUB_KEY_PREF, sizeof))

err:

	/*TODO: cleanup sign_ctx before return */

	if (HERMES_SUCCESS != res)
	{
		free(session_ctx->peer_id);
		session_ctx->peer_id = NULL;
	}

	return res;
}
