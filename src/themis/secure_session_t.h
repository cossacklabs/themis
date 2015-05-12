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

#ifndef THEMIS_SECURE_SESSION_T_H
#define THEMIS_SECURE_SESSION_T_H

#include <soter/soter_t.h>
#include <themis/themis.h>

struct secure_session_type
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


#endif /* THEMIS_SECURE_SESSION_T_H */
