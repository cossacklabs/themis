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

/**
 * @internal
 * @file sym_enc_message.h
 * @brief Secure Cell data layout (Seal and Token Protect, with passphrase)
 *
 * @warning Structures and functions declared in this file are considered
 * implementation details and may change without notice.
 */

#include <themis/themis_error.h>
#include <themis/themis_portable_endian.h>

/**
 * @internal
 * @page secure-cell-data-formats
 * @ingroup THEMIS_SECURE_CELL
 * @subsection seal-master-key Authentication Token for master keys
 *
 * Secure Cell uses the same header format in Seal and Token Protect mode.
 * The difference is that in Seal mode the header is prepended to encrypted
 * message data while in Token Protect mode the header is given to the user
 * as an _authentication token_ that can be stored and transmitted separately
 * from encrypted message data (which has the same length as the plaintext).
 *
 * You can compare master key header to passphrase header. The difference
 * is that master key API uses fixed KDF parameters and thus does not use
 * additional KDF context data.
 *
 * Data layout of the header looks like this:
 *
 * ```
 *     0        1        2        3        4        5        6        7
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |           algorithm ID            |             IV length             |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |          auth tag length          |          message length           |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 *
 *  Initialization Vector data (12 bytes)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+--------+--------+--------+--------+
 * |                                   |
 * +--------+--------+--------+--------+
 *
 *  Authentication Tag data (16 bytes)
 *                                     +--------+--------+--------+--------+
 *                                     |                                   |
 * +--------+--------+--------+--------+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+--------+--------+--------+--------+
 * |                                   |
 * +--------+--------+--------+--------+
 *
 * - - - - >8 - - - - cut here to get Token Protect mode - - - - >8 - - - -
 *
 *  Message data (arbitrary, up to 4 GB)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * ```
 *
 * All numerical fields are unsigned integers encoded in little-endian format.
 *
 * @see Themis RFC 2 contains detailed reference on data formats.
 */

/**
 * Authentication token used by Seal and Token Protect modes with master key.
 *
 * This struct provides only API.
 * It does **not** represent actual in-memory data layout.
 */
struct themis_scell_auth_token_key {
    uint32_t alg;
    const uint8_t* iv;
    uint32_t iv_length;
    const uint8_t* auth_tag;
    uint32_t auth_tag_length;
    uint32_t message_length;
};

static const uint64_t themis_scell_auth_token_key_min_size = 4 * sizeof(uint32_t);

static inline uint64_t themis_scell_auth_token_key_size(const struct themis_scell_auth_token_key* hdr)
{
    uint64_t total_size = 0;
    /* Add separately to avoid overflows in intermediade calculations */
    total_size += sizeof(hdr->alg);
    total_size += sizeof(hdr->iv_length);
    total_size += hdr->iv_length;
    total_size += sizeof(hdr->auth_tag_length);
    total_size += hdr->auth_tag_length;
    total_size += sizeof(hdr->message_length);
    return total_size;
}

static inline themis_status_t themis_write_scell_auth_token_key(
    const struct themis_scell_auth_token_key* hdr, uint8_t* buffer, size_t buffer_length)
{
    if (buffer_length < themis_scell_auth_token_key_size(hdr)) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    buffer = stream_write_uint32LE(buffer, hdr->alg);
    buffer = stream_write_uint32LE(buffer, hdr->iv_length);
    buffer = stream_write_uint32LE(buffer, hdr->auth_tag_length);
    buffer = stream_write_uint32LE(buffer, hdr->message_length);
    buffer = stream_write_bytes(buffer, hdr->iv, hdr->iv_length);
    buffer = stream_write_bytes(buffer, hdr->auth_tag, hdr->auth_tag_length);
    return THEMIS_SUCCESS;
}

static inline themis_status_t themis_read_scell_auth_token_key(const uint8_t* buffer,
                                                               size_t buffer_length,
                                                               struct themis_scell_auth_token_key* hdr)
{
    uint64_t need_length = themis_scell_auth_token_key_min_size;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_uint32LE(buffer, &hdr->alg);
    buffer = stream_read_uint32LE(buffer, &hdr->iv_length);
    buffer = stream_read_uint32LE(buffer, &hdr->auth_tag_length);
    buffer = stream_read_uint32LE(buffer, &hdr->message_length);
    /* Add separately to avoid overflows in intermediate calculations */
    need_length += hdr->iv_length;
    need_length += hdr->auth_tag_length;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_bytes(buffer, &hdr->iv, hdr->iv_length);
    buffer = stream_read_bytes(buffer, &hdr->auth_tag, hdr->auth_tag_length);
    return THEMIS_SUCCESS;
}

static inline themis_status_t themis_scell_auth_token_key_message_size(const uint8_t* auth_token,
                                                                       size_t auth_token_length,
                                                                       uint32_t* message_length)
{
    THEMIS_CHECK_PARAM(message_length != NULL);
    if (auth_token_length < themis_scell_auth_token_key_min_size) {
        return THEMIS_FAIL;
    }
    const uint8_t* message_length_ptr = auth_token + 3 * sizeof(uint32_t);
    stream_read_uint32LE(message_length_ptr, message_length);
    return THEMIS_SUCCESS;
}

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
                                              uint32_t* auth_tag_length);

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
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                uint8_t* auth_token,
                                                size_t* auth_token_length,
                                                uint8_t* encrypted_message,
                                                size_t* encrypted_message_length);

themis_status_t themis_auth_sym_decrypt_message(const uint8_t* key,
                                                size_t key_length,
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                const uint8_t* auth_token,
                                                size_t auth_token_length,
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

#define THEMIS_AUTH_SYM_MAX_KDF_CONTEXT_LENGTH sizeof(uint64_t)

themis_status_t themis_auth_sym_kdf_context(uint32_t message_length,
                                            uint8_t* kdf_context,
                                            size_t* kdf_context_length);

themis_status_t themis_auth_sym_derive_encryption_key(uint32_t soter_alg,
                                                      const uint8_t* key,
                                                      size_t key_length,
                                                      const uint8_t* kdf_context,
                                                      size_t kdf_context_length,
                                                      const uint8_t* user_context,
                                                      size_t user_context_length,
                                                      uint8_t* derived_key,
                                                      size_t* derived_key_length);

#endif /* THEMIS_SYM_ENC_MESSAGE_H */
