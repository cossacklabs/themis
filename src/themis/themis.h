/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS_H
#define THEMIS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef int themis_status_t; 

themis_status_t themis_gen_key_pair(uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length);

themis_status_t wrap_message(const uint8_t* message,
			     const uint8_t* private_key,
			     const size_t private_key_length,
			     const uint8_t* peer_public_key,
			     const size_t peer_public_key_length,
			     uint8_t* protected_message,
			     size_t* protected_message_length);

themis_status_t unwrap_message(const uint8_t* protected_message,
			       const size_t protected_message_length,
			       const uint8_t* private_key,
			       const size_t private_key_length,
			       const uint8_t* peer_public_key,
			       const size_t peer_public_key_length,
			       uint8_t* message,
			       size_t* message_length);

typedef void (*send_protocol_data_callback)(const uint8_t *data, size_t data_length, void *user_data);
typedef void (*protocol_state_changed_callback)(int event, void *user_data);
typedef int (*get_public_key_for_id_callback)(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data);

struct secure_session_user_callbacks_type
{
	send_protocol_data_callback send_data;
	protocol_state_changed_callback state_changed;
	get_public_key_for_id_callback get_public_key_for_id;

	void *user_data;
};

typedef struct secure_session_user_callbacks_type secure_session_user_callbacks_t;


/* TODO: probably move this to private headers */
#include <soter/soter_t.h>
struct secure_session_type
{
	soter_asym_ka_t ecdh_ctx;
	const secure_session_user_callbacks_t *user_callbacks;
	uint8_t *id;
	size_t id_length;
	uint8_t *sign_key;
	size_t sign_key_length;

	uint8_t *peer_id;
	size_t peer_id_length;
	uint8_t *peer_key;
	size_t peer_key_length;
};

typedef struct secure_session_type secure_session_t;

themis_status_t secure_session_init(secure_session_t *session_ctx, const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks);
themis_status_t secure_session_cleanup(secure_session_t *session_ctx);

themis_status_t secure_session_connect(secure_session_t *session_ctx);
themis_status_t secure_session_accept(secure_session_t *session_ctx, const void *data, size_t data_length);
themis_status_t secure_session_proceed(secure_session_t *session_ctx, const void *data, size_t data_length);

themis_status_t secure_session_wrap(secure_session_t *session_ctx, const void *message, size_t message_length, void *wrapped_message, size_t *wrapped_message_length);
themis_status_t secure_session_uwrap(secure_session_t *session_ctx, const void *wrapped_message, size_t wrapped_message_length, void *message, size_t *message_length);

themis_status_t secure_session_send(secure_session_t *session_ctx, const void *message, size_t message_length);
#define secure_session_receive secure_session_uwrap

#endif /* THEMIS_H */



















