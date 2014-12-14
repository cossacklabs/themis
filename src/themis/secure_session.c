/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session.h>
#include <common/error.h>
#include <soter/soter_rsa_key.h>
#include <soter/soter_ec_key.h>

#include <string.h>

#include <themis/secure_session_utils.h>

#define SESSION_ID_GENERATION_LABEL "Themis secure session unique identifier"
#define SESSION_MASTER_KEY_GENERATION_LABEL "Themis secure session master key"

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

	soter_container_hdr_t *container;

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

	length_to_send = 2 * sizeof(soter_container_hdr_t) + session_ctx->we.id_length + ecdh_key_length + signature_length;
	data_to_send = malloc(length_to_send);
	if (NULL == data_to_send)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	/* Storing ID in a container */
	container = (soter_container_hdr_t *)data_to_send + 1;

	memcpy(container->tag, THEMIS_SESSION_ID_TAG, SOTER_CONTAINER_TAG_LENGTH);
	soter_container_set_data_size(container, session_ctx->we.id_length);
	memcpy(soter_container_data(container), session_ctx->we.id, session_ctx->we.id_length);
	soter_update_container_checksum(container);

	/* Moving back to beginning of allocated buffer */
	container = (soter_container_hdr_t *)data_to_send;

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length, &ecdh_key_length, false);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	sign_data.data = data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length;
	sign_data.length = ecdh_key_length;

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, &sign_data, 1, data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length + ecdh_key_length, &signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	memcpy(container->tag, THEMIS_SESSION_PROTO_TAG, SOTER_CONTAINER_TAG_LENGTH);
	soter_container_set_data_size(container, length_to_send - sizeof(soter_container_hdr_t));
	soter_update_container_checksum(container);

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
	const soter_container_hdr_t *proto_message = data;
	const soter_container_hdr_t *peer_id;

	const soter_container_hdr_t *peer_ecdh_key;
	size_t peer_ecdh_key_length;

	const uint8_t *signature;
	size_t signature_length;

	themis_status_t res = HERMES_SUCCESS;
	soter_status_t soter_status;

	uint8_t sign_key[1024]; /* Should be enough for RSA 8192 which is 512 bytes */
	size_t sign_key_length;

	const soter_container_hdr_t *peer_sign_key;
	data_buf_t sign_data[4];

	uint8_t *data_to_send = NULL;
	size_t length_to_send;

	size_t ecdh_key_length = 0;
	soter_container_hdr_t *container;

	if (data_length < sizeof(soter_container_hdr_t))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (memcmp(proto_message->tag, THEMIS_SESSION_PROTO_TAG, SOTER_CONTAINER_TAG_LENGTH))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (data_length < (soter_container_data_size(proto_message) + sizeof(soter_container_hdr_t)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (HERMES_SUCCESS != soter_verify_container_checksum(proto_message))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_id = (const soter_container_hdr_t *)soter_container_const_data(proto_message);

	if (memcmp(peer_id->tag, THEMIS_SESSION_ID_TAG, SOTER_CONTAINER_TAG_LENGTH))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (HERMES_SUCCESS != soter_verify_container_checksum(peer_id))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_ecdh_key = (const soter_container_hdr_t *)(soter_container_const_data(peer_id) + soter_container_data_size(peer_id));

	if (memcmp(peer_ecdh_key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_ecdh_key_length = ntohl(peer_ecdh_key->size);

	signature = soter_container_const_data(peer_ecdh_key) + peer_ecdh_key_length;
	signature_length = (const uint8_t *)data + soter_container_data_size(proto_message) + sizeof(soter_container_hdr_t) - signature;

	if (session_ctx->user_callbacks->get_public_key_for_id(soter_container_const_data(peer_id), soter_container_data_size(peer_id), sign_key, sizeof(sign_key), session_ctx->user_callbacks->user_data))
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

	sign_data[0].data = (const uint8_t *)peer_ecdh_key;
	sign_data[0].length = peer_ecdh_key_length;

	res = verify_signature(peer_sign_key, sign_key_length, sign_data, 1, signature, signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	res = secure_session_peer_init(&(session_ctx->peer), soter_container_const_data(peer_id), soter_container_data_size(peer_id), peer_ecdh_key, peer_ecdh_key_length, peer_sign_key, sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	/* Preparing to send response */
	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, NULL, 0, NULL, &signature_length);
	if (HERMES_BUFFER_TOO_SMALL != res)
	{
		goto err;
	}

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), NULL, &ecdh_key_length, false);
	if (HERMES_BUFFER_TOO_SMALL != soter_status)
	{
		res = soter_status;
		goto err;
	}

	length_to_send = 2 * sizeof(soter_container_hdr_t) + session_ctx->we.id_length + ecdh_key_length + signature_length;
	data_to_send = malloc(length_to_send);
	if (NULL == data_to_send)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	/* Storing ID in a container */
	container = (soter_container_hdr_t *)data_to_send + 1;

	memcpy(container->tag, THEMIS_SESSION_ID_TAG, SOTER_CONTAINER_TAG_LENGTH);
	soter_container_set_data_size(container, session_ctx->we.id_length);
	memcpy(soter_container_data(container), session_ctx->we.id, session_ctx->we.id_length);
	soter_update_container_checksum(container);

	/* Moving back to beginning of allocated buffer */
	container = (soter_container_hdr_t *)data_to_send;

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length, &ecdh_key_length, false);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	sign_data[0].data = data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length;
	sign_data[0].length = ecdh_key_length;

	sign_data[1].data = session_ctx->peer.ecdh_key;
	sign_data[1].length = session_ctx->peer.ecdh_key_length;

	sign_data[2].data = session_ctx->we.id;
	sign_data[2].length = session_ctx->we.id_length;

	sign_data[3].data = session_ctx->peer.id;
	sign_data[3].length = session_ctx->peer.id_length;

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, sign_data, 4, data_to_send + (2 * sizeof(soter_container_hdr_t)) + session_ctx->we.id_length + ecdh_key_length, &signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	memcpy(container->tag, THEMIS_SESSION_PROTO_TAG, SOTER_CONTAINER_TAG_LENGTH);
	soter_container_set_data_size(container, length_to_send - sizeof(soter_container_hdr_t));
	soter_update_container_checksum(container);

	session_ctx->user_callbacks->send_data(data_to_send, length_to_send, session_ctx->user_callbacks->user_data);

err:

	if (data_to_send)
	{
		free(data_to_send);
	}

	if (HERMES_SUCCESS != res)
	{
		secure_session_peer_cleanup(&(session_ctx->peer));
	}

	return res;
}

themis_status_t secure_session_proceed_client(secure_session_t *session_ctx, const void *data, size_t data_length)
{
	const soter_container_hdr_t *proto_message = data;
	const soter_container_hdr_t *peer_id;

	const soter_container_hdr_t *peer_ecdh_key;
	size_t peer_ecdh_key_length;

	const uint8_t *signature;
	size_t signature_length;

	themis_status_t res = HERMES_SUCCESS;
	soter_status_t soter_status;

	uint8_t sign_key[1024]; /* Should be enough for RSA 8192 which is 512 bytes */
	size_t sign_key_length;

	uint8_t ecdh_key[1024];
	size_t ecdh_key_length = sizeof(ecdh_key);

	const soter_container_hdr_t *peer_sign_key;
	data_buf_t sign_data[4];

	uint8_t *data_to_send = NULL;
	size_t length_to_send;

	soter_container_hdr_t *container;

	if (data_length < sizeof(soter_container_hdr_t))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (memcmp(proto_message->tag, THEMIS_SESSION_PROTO_TAG, SOTER_CONTAINER_TAG_LENGTH))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (data_length < (soter_container_data_size(proto_message) + sizeof(soter_container_hdr_t)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (HERMES_SUCCESS != soter_verify_container_checksum(proto_message))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_id = (const soter_container_hdr_t *)soter_container_const_data(proto_message);

	if (memcmp(peer_id->tag, THEMIS_SESSION_ID_TAG, SOTER_CONTAINER_TAG_LENGTH))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (HERMES_SUCCESS != soter_verify_container_checksum(peer_id))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_ecdh_key = (const soter_container_hdr_t *)(soter_container_const_data(peer_id) + soter_container_data_size(peer_id));

	if (memcmp(peer_ecdh_key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
	{
		return HERMES_INVALID_PARAMETER;
	}

	peer_ecdh_key_length = ntohl(peer_ecdh_key->size);

	signature = soter_container_const_data(peer_ecdh_key) + peer_ecdh_key_length;
	signature_length = (const uint8_t *)data + soter_container_data_size(proto_message) + sizeof(soter_container_hdr_t) - signature;

	if (session_ctx->user_callbacks->get_public_key_for_id(soter_container_const_data(peer_id), soter_container_data_size(peer_id), sign_key, sizeof(sign_key), session_ctx->user_callbacks->user_data))
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

	soter_status = soter_asym_ka_export_key(&(session_ctx->ecdh_ctx), ecdh_key, &ecdh_key_length, false);
	if (HERMES_SUCCESS != soter_status)
	{
		res = soter_status;
		goto err;
	}

	sign_data[0].data = (const uint8_t *)peer_ecdh_key;
	sign_data[0].length = peer_ecdh_key_length;

	sign_data[1].data = ecdh_key;
	sign_data[1].length = ecdh_key_length;

	sign_data[2].data = soter_container_const_data(peer_id);
	sign_data[2].length = soter_container_data_size(peer_id);

	sign_data[3].data = session_ctx->we.id;
	sign_data[3].length = session_ctx->we.id_length;

	res = verify_signature(peer_sign_key, sign_key_length, sign_data, 4, signature, signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	res = secure_session_peer_init(&(session_ctx->peer), soter_container_const_data(peer_id), soter_container_data_size(peer_id), peer_ecdh_key, peer_ecdh_key_length, peer_sign_key, sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	sign_data[0].data = ecdh_key;
	sign_data[0].length = ecdh_key_length;

	sign_data[1].data = (const uint8_t *)peer_ecdh_key;
	sign_data[1].length = peer_ecdh_key_length;

	sign_data[2].data = session_ctx->we.id;
	sign_data[2].length = session_ctx->we.id_length;

	sign_data[3].data = soter_container_const_data(peer_id);
	sign_data[3].length = soter_container_data_size(peer_id);

	res = themis_kdf(NULL, 0, SESSION_ID_GENERATION_LABEL, sign_data, 4, &(session_ctx->session_id), sizeof(session_ctx->session_id));
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	/* we will reuse sign_key buffer for shared secret computation */
	sign_key_length = sizeof(sign_key);
	res = soter_asym_ka_derive(&(session_ctx->ecdh_ctx), peer_ecdh_key, peer_ecdh_key_length, sign_key, &sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	sign_data[0].data = (const uint8_t *)(&(session_ctx->session_id));
	sign_data[0].length = sizeof(session_ctx->session_id);

	res = themis_kdf(sign_key, sign_key_length, SESSION_MASTER_KEY_GENERATION_LABEL, sign_data, 1, session_ctx->session_master_key, sizeof(session_ctx->session_master_key));
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	/* restore sign data for signature computation */
	sign_data[0].data = ecdh_key;
	sign_data[0].length = ecdh_key_length;

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, NULL, 0, NULL, &signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	/* we will reuse sign_key_length for mac length retrieval */
	res = compute_mac(session_ctx->session_master_key, sizeof(session_ctx->session_master_key), NULL, 0, NULL, &sign_key_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	length_to_send = signature_length + sign_key_length;
	data_to_send = malloc(length_to_send);
	if (NULL == data_to_send)
	{
		res = HERMES_NO_MEMORY;
		goto err;
	}

	res = compute_signature(session_ctx->we.sign_key, session_ctx->we.sign_key_length, sign_data, 4, data_to_send, &signature_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	sign_data[0].data = (const uint8_t *)peer_ecdh_key;
	sign_data[0].length = peer_ecdh_key_length;

	sign_data[1].data = (const uint8_t *)(&(session_ctx->session_id));
	sign_data[1].length = sizeof(session_ctx->session_id);

	res = compute_mac(session_ctx->session_master_key, sizeof(session_ctx->session_master_key), sign_data, 2, data_to_send + signature_length, &sign_key_length);
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

	if (HERMES_SUCCESS != res)
	{
		secure_session_peer_cleanup(&(session_ctx->peer));
	}

	return res;
}
