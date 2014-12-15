/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS_SECURE_SESSION_H
#define THEMIS_SECURE_SESSION_H

#include <themis/themis.h>

#define THEMIS_SESSION_ID_TAG "TSID"
#define THEMIS_SESSION_PROTO_TAG "TSPM"

typedef void (*send_protocol_data_callback)(const uint8_t *data, size_t data_length, void *user_data);
typedef ssize_t (*receive_protocol_data_callback)(uint8_t *data, size_t data_length, void *user_data);
typedef void (*protocol_state_changed_callback)(int event, void *user_data);
typedef int (*get_public_key_for_id_callback)(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data);

struct secure_session_user_callbacks_type
{
	send_protocol_data_callback send_data;
	receive_protocol_data_callback receive_data;
	protocol_state_changed_callback state_changed;
	get_public_key_for_id_callback get_public_key_for_id;

	void *user_data;
};

typedef struct secure_session_user_callbacks_type secure_session_user_callbacks_t;


/* TODO: probably move this to private headers */
#include <soter/soter_t.h>
#include <themis/secure_session_peer.h>
/* TODO: move to separate header */
struct secure_session_peer_type
{
	uint8_t *id;
	size_t id_length;

	uint8_t *ecdh_key;
	size_t ecdh_key_length;

	uint8_t *sign_key;
	size_t sign_key_length;
};

typedef struct secure_session_peer_type secure_session_peer_t;

themis_status_t secure_session_peer_init(secure_session_peer_t *peer, const void *id, size_t id_len, const void *ecdh_key, size_t ecdh_key_len, const void *sign_key, size_t sign_key_len);
void secure_session_peer_cleanup(secure_session_peer_t *peer);

#define SESSION_MASTER_KEY_LENGTH 32

typedef struct secure_session_type secure_session_t;
typedef themis_status_t (*secure_session_handler)(secure_session_t *session_ctx, const void *data, size_t data_length);

struct secure_session_type
{
	soter_asym_ka_t ecdh_ctx;
	const secure_session_user_callbacks_t *user_callbacks;

	secure_session_handler state_handler;

	struct secure_session_peer_type we;
	struct secure_session_peer_type peer;

	uint32_t session_id;
	uint8_t session_master_key[SESSION_MASTER_KEY_LENGTH];
};

themis_status_t secure_session_init(secure_session_t *session_ctx, const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks);
themis_status_t secure_session_cleanup(secure_session_t *session_ctx);

themis_status_t secure_session_connect(secure_session_t *session_ctx);

themis_status_t secure_session_wrap(secure_session_t *session_ctx, const void *message, size_t message_length, void *wrapped_message, size_t *wrapped_message_length);
themis_status_t secure_session_uwrap(secure_session_t *session_ctx, const void *wrapped_message, size_t wrapped_message_length, void *message, size_t *message_length);

themis_status_t secure_session_send(secure_session_t *session_ctx, const void *message, size_t message_length);
ssize_t secure_session_receive(secure_session_t *session_ctx, void *message, size_t message_length);

void themis_test_func(void);

#endif /* THEMIS_SECURE_SESSION_H */
