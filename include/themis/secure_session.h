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
 * @brief Secure session is a lightweight mechanism to secure any network communications (both
 * private and public networks including Internet)
 */
#ifndef THEMIS_SECURE_SESSION_H
#define THEMIS_SECURE_SESSION_H

/*
 * ssize_t is POSIX-specific (as is <sys/types.h>).
 * Explicitly define ssize_t as signed counterpart of size_t on Windows.
 */
#if _WIN32
#include <stddef.h>
#include <stdint.h>
#ifdef _WIN64
typedef signed __int64 ssize_t;
#else
typedef signed int ssize_t;
#endif
#else
#include <sys/types.h>
#endif

#include <soter/soter.h>

#include <themis/themis_api.h>
#include <themis/themis_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup THEMIS
 * @{
 * @defgroup THEMIS_SECURE_SESSION secure session
 * @brief Secure session is a lightweight mechanism to secure any network communications (both
 * private and public networks including Internet)
 * @{
 */

/** @brief idle state define */
#define STATE_IDLE 0
/** @brief negotiating state define */
#define STATE_NEGOTIATING 1
/** @brief established state define */
#define STATE_ESTABLISHED 2

/** @brief send data callback typedef*/
typedef ssize_t (*send_protocol_data_callback)(const uint8_t* data, size_t data_length, void* user_data);
/** @brief receive data callback typedef*/
typedef ssize_t (*receive_protocol_data_callback)(uint8_t* data, size_t data_length, void* user_data);
/** @brief state change callback typedef*/
typedef void (*protocol_state_changed_callback)(int event, void* user_data);
/** @brief get public key by id callback typedef*/
typedef int (*get_public_key_for_id_callback)(
    const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data);

struct secure_session_user_callbacks_type {
    send_protocol_data_callback send_data;
    receive_protocol_data_callback receive_data;
    protocol_state_changed_callback state_changed;
    get_public_key_for_id_callback get_public_key_for_id;

    void* user_data;
};

typedef struct secure_session_user_callbacks_type secure_session_user_callbacks_t;

typedef struct secure_session_type secure_session_t;

THEMIS_API
secure_session_t* secure_session_create(const void* id,
                                        size_t id_length,
                                        const void* sign_key,
                                        size_t sign_key_length,
                                        const secure_session_user_callbacks_t* user_callbacks);

THEMIS_API
themis_status_t secure_session_destroy(secure_session_t* session_ctx);

THEMIS_API
themis_status_t secure_session_connect(secure_session_t* session_ctx);

THEMIS_API
themis_status_t secure_session_generate_connect_request(secure_session_t* session_ctx,
                                                        void* output,
                                                        size_t* output_length);

THEMIS_API
themis_status_t secure_session_wrap(secure_session_t* session_ctx,
                                    const void* message,
                                    size_t message_length,
                                    void* wrapped_message,
                                    size_t* wrapped_message_length);

THEMIS_API
themis_status_t secure_session_unwrap(secure_session_t* session_ctx,
                                      const void* wrapped_message,
                                      size_t wrapped_message_length,
                                      void* message,
                                      size_t* message_length);

/* Trying to mimic socket functions */
THEMIS_API
ssize_t secure_session_send(secure_session_t* session_ctx, const void* message, size_t message_length);

THEMIS_API
ssize_t secure_session_receive(secure_session_t* session_ctx, void* message, size_t message_length);

THEMIS_API
themis_status_t secure_session_save(const secure_session_t* session_ctx, void* out, size_t* out_length);

THEMIS_API
themis_status_t secure_session_load(secure_session_t* session_ctx,
                                    const void* in,
                                    size_t in_length,
                                    const secure_session_user_callbacks_t* user_callbacks);

THEMIS_API
bool secure_session_is_established(const secure_session_t* session_ctx);

THEMIS_API
themis_status_t secure_session_get_remote_id(const secure_session_t* session_ctx,
                                             uint8_t* id,
                                             size_t* id_length);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* THEMIS_SECURE_SESSION_H */
