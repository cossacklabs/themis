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

/**
 * @file secure_session.h
 * @brief Secure session is a lightweight mechanism to secure any network communications (both private and public networks including Internet)
 */
#ifndef THEMIS_SECURE_SESSION_H
#define THEMIS_SECURE_SESSION_H

#include <themis/themis.h>

#include <sys/types.h>
#include <soter/soter.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_SESSION secure session
 * @brief Secure session is a lightweight mechanism to secure any network communications (both private and public networks including Internet)
 * @{
 */

/** @brief id tag */
#define THEMIS_SESSION_ID_TAG "TSID"
/** @brief protocol tag */
#define THEMIS_SESSION_PROTO_TAG "TSPM"

/** @brief idle state define */
#define STATE_IDLE 0 
/** @brief negotiating state define */
#define STATE_NEGOTIATING 1
/** @brief established state define */
#define STATE_ESTABLISHED 2

/** @brief send data callbeck tyoedef*/
typedef ssize_t (*send_protocol_data_callback)(const uint8_t *data, size_t data_length, void *user_data);
/** @brief receive data callbeck tyoedef*/
typedef ssize_t (*receive_protocol_data_callback)(uint8_t *data, size_t data_length, void *user_data);
/** @brief state change callbeck tyoedef*/
typedef void (*protocol_state_changed_callback)(int event, void *user_data);
/** @brief get public key by id callbeck tyoedef*/
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
//#include <soter/soter_t.h>
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
/* TODO: for now session keys are same length as master key */
#define SESSION_MESSAGE_KEY_LENGTH SESSION_MASTER_KEY_LENGTH

typedef struct secure_session_type secure_session_t;
typedef themis_status_t (*secure_session_handler)(secure_session_t *session_ctx, const void *data, size_t data_length, void *output, size_t *output_length);

/*struct secure_session_type
{
	soter_asym_ka_t ecdh_ctx;
	const secure_session_user_callbacks_t *user_callbacks;

	secure_session_handler state_handler;

	struct secure_session_peer_type we;
	struct secure_session_peer_type peer;

	uint32_t session_id;
	uint8_t session_master_key[SESSION_MASTER_KEY_LENGTH];

	uint8_t out_cipher_key[SESSION_MESSAGE_KEY_LENGTH];
	uint8_t in_cipher_key[SESSION_MESSAGE_KEY_LENGTH];

	uint32_t out_seq;
	uint32_t in_seq;

	bool is_client;
};

themis_status_t secure_session_init(secure_session_t *session_ctx, const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks);
themis_status_t secure_session_cleanup(secure_session_t *session_ctx);
*/
secure_session_t* secure_session_create(const void *id, size_t id_length, const void *sign_key, size_t sign_key_length, const secure_session_user_callbacks_t *user_callbacks);
themis_status_t secure_session_destroy(secure_session_t *session_ctx);

themis_status_t secure_session_connect(secure_session_t *session_ctx);
themis_status_t secure_session_generate_connect_request(secure_session_t *session_ctx, void *output, size_t *output_length);

themis_status_t secure_session_wrap(secure_session_t *session_ctx, const void *message, size_t message_length, void *wrapped_message, size_t *wrapped_message_length);
themis_status_t secure_session_unwrap(secure_session_t *session_ctx, const void *wrapped_message, size_t wrapped_message_length, void *message, size_t *message_length);

/* Trying to mimic socket functions */
ssize_t secure_session_send(secure_session_t *session_ctx, const void *message, size_t message_length);
ssize_t secure_session_receive(secure_session_t *session_ctx, void *message, size_t message_length);

themis_status_t secure_session_save(const secure_session_t *session_ctx, void *out, size_t *out_length);
themis_status_t secure_session_load(secure_session_t *session_ctx, const void *in, size_t in_length, const secure_session_user_callbacks_t *user_callbacks);

bool secure_session_is_established(const secure_session_t *session_ctx);


/** @} */
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* THEMIS_SECURE_SESSION_H */
