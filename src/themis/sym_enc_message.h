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

#ifndef THEMIS_SYM_ENC_MESSAGE_H
#define THEMIS_SYM_ENC_MESSAGE_H

#include <themis/themis_error.h>

themis_status_t themis_auth_sym_plain_encrypt(uint32_t alg,
                                              const uint8_t* key,
                                              size_t key_length,
                                              const uint8_t* iv,
                                              size_t iv_length,
                                              const uint8_t* aad,
                                              size_t aad_length,
                                              const uint8_t* message,
                                              size_t message_length,
                                              uint8_t* encrypted_message,
                                              size_t* encrypted_message_length,
                                              uint8_t* auth_tag,
                                              size_t* auth_tag_length);

themis_status_t themis_auth_sym_plain_decrypt(uint32_t alg,
                                              const uint8_t* key,
                                              size_t key_length,
                                              const uint8_t* iv,
                                              size_t iv_length,
                                              const uint8_t* aad,
                                              size_t aad_length,
                                              const uint8_t* encrypted_message,
                                              size_t encrypted_message_length,
                                              uint8_t* message,
                                              size_t* message_length,
                                              const uint8_t* auth_tag,
                                              size_t auth_tag_length);

themis_status_t themis_auth_sym_encrypt_message(const uint8_t* key,
                                                size_t key_length,
                                                const uint8_t* message,
                                                size_t message_length,
                                                const uint8_t* in_context,
                                                size_t in_context_length,
                                                uint8_t* out_context,
                                                size_t* out_context_length,
                                                uint8_t* encrypted_message,
                                                size_t* encrypted_message_length);

themis_status_t themis_auth_sym_decrypt_message(const uint8_t* key,
                                                size_t key_length,
                                                const uint8_t* in_context,
                                                size_t in_context_length,
                                                const uint8_t* context,
                                                size_t context_length,
                                                const uint8_t* encrypted_message,
                                                size_t encrypted_message_length,
                                                uint8_t* message,
                                                size_t* message_length);

themis_status_t themis_sym_encrypt_message_u(const uint8_t* key,
                                             size_t key_length,
                                             const uint8_t* context,
                                             size_t context_length,
                                             const uint8_t* message,
                                             size_t message_length,
                                             uint8_t* encrypted_message,
                                             size_t* encrypted_message_length);

themis_status_t themis_sym_decrypt_message_u(const uint8_t* key,
                                             size_t key_length,
                                             const uint8_t* context,
                                             size_t context_length,
                                             const uint8_t* encrypted_message,
                                             size_t encrypted_message_length,
                                             uint8_t* message,
                                             size_t* message_length);
#endif /* THEMIS_SYM_ENC_MESSAGE_H */
