/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session.h>
#include <themis/secure_session_utils.h>
#include <common/error.h>

#include <soter/soter_container.h>

#include <string.h>

#include <arpa/inet.h>
#include "portable_endian.h"
#define THEMIS_SESSION_WRAP_TAG "TSWM"

/* How much sequence number may differ from expected for message to be considered valid */
#define SEQ_MAX_DIFF 8

/* How much timestamp may differ from expected for message to be considered valid */
#define TS_MAX_DIFF 86400 /* One day */

themis_status_t secure_session_wrap(secure_session_t *session_ctx, const void *message, size_t message_length, void *wrapped_message, size_t *wrapped_message_length)
{
	uint32_t *session_id = (uint32_t *)wrapped_message;
	uint8_t *iv = (uint8_t *)(session_id + 1);
	uint32_t *length = (uint32_t *)(iv + CIPHER_MAX_BLOCK_SIZE);
	uint32_t *seq = length + 1;
	uint8_t *ts = (uint8_t *)(seq + 1);

	themis_status_t res;

	if ((NULL == session_ctx) || (NULL == message) || (0 == message_length) || (NULL == wrapped_message_length))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if ((NULL == wrapped_message) || (WRAPPED_SIZE(message_length) > *wrapped_message_length))
	{
		*wrapped_message_length = WRAPPED_SIZE(message_length);
		return HERMES_BUFFER_TOO_SMALL;
	}

	if (-1 == time((time_t *)ts))
	{
		return HERMES_FAIL;
	}

	*((time_t *)ts) = htobe64(*((time_t *)ts));

	*wrapped_message_length = WRAPPED_SIZE(message_length);
	memmove(ts + 8, message, message_length);

	*seq = htonl(session_ctx->out_seq);
	*length = htonl(message_length + sizeof(uint32_t) + sizeof(time_t));

	res = soter_rand(iv, CIPHER_MAX_BLOCK_SIZE);
	if (HERMES_SUCCESS != res)
	{
		return res;
	}

	res = encrypt_gcm(session_ctx->out_cipher_key, sizeof(session_ctx->out_cipher_key), iv, CIPHER_MAX_BLOCK_SIZE, length, message_length + sizeof(uint32_t) + sizeof(time_t) + sizeof(uint32_t), length, message_length + sizeof(uint32_t) + sizeof(time_t) + sizeof(uint32_t) + CIPHER_AUTH_TAG_SIZE);
	if (HERMES_SUCCESS != res)
	{
		return res;
	}

	*session_id = htonl(session_ctx->session_id);
	session_ctx->out_seq++;

	return HERMES_SUCCESS;
}

themis_status_t secure_session_unwrap(secure_session_t *session_ctx, const void *wrapped_message, size_t wrapped_message_length, void *message, size_t *message_length)
{
	const uint32_t *session_id = (const uint32_t *)wrapped_message;
	const uint8_t *iv = (const uint8_t *)(session_id + 1);

	uint8_t message_header[sizeof(uint32_t) + sizeof(uint32_t) + sizeof(time_t)];
	size_t message_header_size = sizeof(message_header);
	uint32_t length;
	uint32_t seq;
	time_t ts;

	time_t curr_time;
	themis_status_t res;

	soter_sym_ctx_t *sym_ctx;

	/* TODO: needs refactoring */
	if (!secure_session_is_established(session_ctx))
	{
		res = session_ctx->state_handler(session_ctx, wrapped_message, wrapped_message_length, message, message_length);
		if ((HERMES_SUCCESS == res) && (*message_length > 0))
		{
			return HERMES_SSESSION_SEND_OUTPUT_TO_PEER;
		}

		return res;
	}

	if ((NULL == session_ctx) || (NULL == wrapped_message) || (WRAP_AUX_DATA > wrapped_message_length) || (NULL == message_length))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if ((NULL == message) || (UNWRAPPED_SIZE(wrapped_message_length) > *message_length))
	{
		*message_length = UNWRAPPED_SIZE(wrapped_message_length);
		return HERMES_BUFFER_TOO_SMALL;
	}

	curr_time = time(NULL);
	if (-1 == curr_time)
	{
		return HERMES_FAIL;
	}

	if (ntohl(*session_id) != session_ctx->session_id)
	{
		return HERMES_INVALID_PARAMETER;
	}

	sym_ctx = soter_sym_aead_decrypt_create(SOTER_SYM_AES_GCM|SOTER_SYM_256_KEY_LENGTH, session_ctx->in_cipher_key, sizeof(session_ctx->in_cipher_key), NULL, 0,iv, CIPHER_MAX_BLOCK_SIZE);
	if (NULL == sym_ctx)
	{
		return HERMES_FAIL;
	}

	/* TODO: change to GCM when fixed */
	/*{
		size_t i;

		for (i = 0; i < sizeof(message_header); i++)
		{
			message_header[i] = iv[CIPHER_MAX_BLOCK_SIZE + i] ^ 0xff;
		}

		length = ntohl(*((uint32_t *)message_header));
		seq = ntohl(*((uint32_t *)(message_header + sizeof(uint32_t))));
		ts = be64toh(*((time_t *)(message_header + sizeof(uint32_t) + sizeof(uint32_t))));
	}*/

	res = soter_sym_aead_decrypt_update(sym_ctx, iv + CIPHER_MAX_BLOCK_SIZE, sizeof(message_header), message_header, &message_header_size);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	if (sizeof(message_header) != message_header_size)
	{
		res = HERMES_FAIL;
		goto err;
	}

	length = ntohl(*((uint32_t *)message_header));
	seq = ntohl(*((uint32_t *)(message_header + sizeof(uint32_t))));
	ts = be64toh(*((time_t *)(message_header + sizeof(uint32_t) + sizeof(uint32_t))));

	if (length > (UNWRAPPED_SIZE(wrapped_message_length) + sizeof(uint32_t) + 8))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	if ((seq < (session_ctx->in_seq - SEQ_MAX_DIFF)) || (seq > (session_ctx->in_seq + SEQ_MAX_DIFF)))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	if ((ts < (curr_time - TS_MAX_DIFF)) || (ts > (curr_time + TS_MAX_DIFF)))
	{
		res = HERMES_INVALID_PARAMETER;
		goto err;
	}

	*message_length = length - (sizeof(uint32_t) + sizeof(time_t));

	/* TODO: change to GCM when fixed */
	/*{
		size_t i;

		for (i = 0; i < *message_length; i++)
		{
			((uint8_t *)message)[i] = iv[CIPHER_MAX_BLOCK_SIZE + sizeof(message_header) + i] ^ 0xff;
		}
	}*/

	res = soter_sym_aead_decrypt_update(sym_ctx, iv + CIPHER_MAX_BLOCK_SIZE + sizeof(message_header), *message_length, message, message_length);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	res = soter_sym_aead_decrypt_final(sym_ctx, iv + CIPHER_MAX_BLOCK_SIZE + sizeof(message_header) + *message_length, CIPHER_AUTH_TAG_SIZE);
	if (HERMES_SUCCESS != res)
	{
		goto err;
	}

	session_ctx->in_seq = seq;

err:

    if (NULL != sym_ctx)
	{
		soter_sym_aead_decrypt_destroy(sym_ctx);
	}

	return res;
}
