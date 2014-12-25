/**
 * @file
 *
 * (c) CossackLabs
 */

#include <themis/secure_session.h>
#include <common/error.h>

#include <soter/soter_container.h>

#include <string.h>

#define THEMIS_SESSION_WRAP_TAG "TSWM"

#define CIPHER_MAX_BLOCK_SIZE 16
#define CIPHER_AUTH_TAG_SIZE 16

/* Message size + session id + iv + length + sequence number + timestamp + MAC */
#define WRAP_AUX_DATA (4 + CIPHER_MAX_BLOCK_SIZE + 4 + 4 + 8 + CIPHER_AUTH_TAG_SIZE)

#define WRAPPED_SIZE(_SIZE_) (_SIZE_ +  WRAP_AUX_DATA)
#define UNWRAPPED_SIZE(_WRAP_SIZE_) (_WRAP_SIZE_ -  WRAP_AUX_DATA)

themis_status_t secure_session_wrap(secure_session_t *session_ctx, const void *message, size_t message_length, void *wrapped_message, size_t *wrapped_message_length)
{
	uint8_t *output = wrapped_message;
	uint32_t *session_id = (uint32_t *)output;
	uint8_t *iv = output + sizeof(uint32_t);
	uint32_t *length = (uint32_t *)(iv + CIPHER_MAX_BLOCK_SIZE);
	uint32_t *seq = length + 1;
	uint8_t *ts = (uint8_t *)(seq + 1);

	if ((NULL == session_ctx) || (NULL == message) || (0 == message_length) || (NULL == wrapped_message_length))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if ((NULL == wrapped_message) || (WRAPPED_SIZE(message_length) > *wrapped_message_length))
	{
		*wrapped_message_length = WRAPPED_SIZE(message_length);
		return HERMES_BUFFER_TOO_SMALL;
	}

	*wrapped_message_length = WRAPPED_SIZE(message_length);
	memmove(output + (WRAP_AUX_DATA - CIPHER_AUTH_TAG_SIZE), message, message_length);

	return HERMES_FAIL;
}


