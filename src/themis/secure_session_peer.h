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

#ifndef THEMIS_SECURE_SESSION_PEER_H
#define THEMIS_SECURE_SESSION_PEER_H

#include <stdint.h>
#include <stdlib.h>

#include <themis/secure_session.h>

struct secure_session_peer_type {
    uint8_t* id;
    size_t id_length;

    uint8_t* ecdh_key;
    size_t ecdh_key_length;

    uint8_t* sign_key;
    size_t sign_key_length;
};

typedef struct secure_session_peer_type secure_session_peer_t;

themis_status_t secure_session_peer_init(secure_session_peer_t* peer,
                                         const void* id,
                                         size_t id_len,
                                         const void* ecdh_key,
                                         size_t ecdh_key_len,
                                         const void* sign_key,
                                         size_t sign_key_len);

void secure_session_peer_cleanup(secure_session_peer_t* peer);

#endif /* THEMIS_SECURE_SESSION_PEER_H */
