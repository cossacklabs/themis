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

#ifndef THEMIS_SECURE_SESSION_UTILS_H
#define THEMIS_SECURE_SESSION_UTILS_H

#include <stdint.h>
#include <string.h>

#include <soter/soter.h>
#include <themis/themis.h>
#include <themis/secure_session.h>

#define CIPHER_MAX_BLOCK_SIZE 16
#define CIPHER_AUTH_TAG_SIZE 16

soter_sign_alg_t get_key_sign_type(const void *sign_key, size_t sign_key_length);
soter_sign_alg_t get_peer_key_sign_type(const void *sign_key, size_t sign_key_length);
themis_status_t compute_signature(const void *sign_key, size_t sign_key_length, const soter_kdf_context_buf_t *sign_data, size_t sign_data_count, void *signature, size_t *signature_length);
themis_status_t verify_signature(const void *verify_key, size_t verify_key_length, const soter_kdf_context_buf_t *sign_data, size_t sign_data_count, const void *signature, size_t signature_length);
themis_status_t compute_mac(const void *key, size_t key_length, const soter_kdf_context_buf_t *data, size_t data_count, void *mac, size_t *mac_length);
themis_status_t verify_mac(const void *key, size_t key_length, const soter_kdf_context_buf_t *data_buf_t, size_t data_count, const void *mac, size_t mac_length);
themis_status_t encrypt_gcm(const void *key, size_t key_length, const void *iv, size_t iv_length, const void *in, size_t in_length, void *out, size_t out_length);
themis_status_t decrypt_gcm(const void *key, size_t key_length, const void *iv, size_t iv_length, const void *in, size_t in_length, void *out, size_t out_length);
themis_status_t secure_session_derive_message_keys(secure_session_t *session_ctx);

/* Message size + session id + iv + length + sequence number + timestamp + MAC */
#define WRAP_AUX_DATA (4 + CIPHER_MAX_BLOCK_SIZE + 4 + 4 + 8 + CIPHER_AUTH_TAG_SIZE)

#define WRAPPED_SIZE(_SIZE_) (_SIZE_ +  WRAP_AUX_DATA)
#define UNWRAPPED_SIZE(_WRAP_SIZE_) (_WRAP_SIZE_ -  WRAP_AUX_DATA)

#endif /* THEMIS_SECURE_SESSION_UTILS_H */
