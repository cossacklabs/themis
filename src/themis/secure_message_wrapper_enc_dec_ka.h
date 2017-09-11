/*
 * Copyright (c) 2017 Cossack Labs Limited
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

#ifndef THEMIS_SECURE_MESSAGE_WRAPPER_ENC_DEC_KA_H
#define THEMIS_SECURE_MESSAGE_WRAPPER_ENC_DEC_KA_H

typedef struct themis_secure_message_ka_worker_type themis_secure_message_ka_t;

themis_secure_message_ka_t* themis_secure_message_ka_encrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_ka_encrypter_destroy(themis_secure_message_ka_t* ctx);
themis_status_t themis_secure_message_ka_encrypter_proceed(themis_secure_message_ka_t* ctx, const uint8_t* message, const size_t message_length, uint8_t* wrapped_message, size_t* wrapped_message_length);
themis_secure_message_ka_t* themis_secure_message_ka_decrypter_init(const uint8_t* private_key, const size_t private_key_length, const uint8_t* peer_public_key, const size_t peer_public_key_length);
themis_status_t themis_secure_message_ka_decrypter_proceed(themis_secure_message_ka_t* ctx, const uint8_t* wrapped_message, const size_t wrapped_message_length, uint8_t* message, size_t* message_length);
themis_status_t themis_secure_message_ka_decrypter_destroy(themis_secure_message_ka_t* ctx);

#endif // THEMIS_SECURE_MESSAGE_WRAPPER_ENC_DEC_KA_H
