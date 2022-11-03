/*
 * Copyright (c) 2020 Cossack Labs Limited
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
 * @internal
 * @file secure_cell_seal_passphrase.h
 * @brief Secure Cell data layout (Seal and Token Protect, with passphrase)
 *
 * @warning Structures and functions declared in this file are considered
 * implementation details and may change without notice.
 */

#ifndef THEMIS_SECURE_CELL_SEAL_PASSPHRASE_H
#define THEMIS_SECURE_CELL_SEAL_PASSPHRASE_H

#include <themis/themis_api.h>
#include <themis/themis_error.h>
#include <themis/themis_portable_endian.h>

/**
 * @internal
 * @page secure-cell-data-formats
 * @ingroup THEMIS_SECURE_CELL
 * @subsection seal-passphrase Authentication Token for passphrases
 *
 * Secure Cell uses the same header format in Seal and Token Protect mode.
 * The difference is that in Seal mode the header is prepended to encrypted
 * message data while in Token Protect mode the header is given to the user
 * as an _authentication token_ that can be stored and transmitted separately
 * from encrypted message data (which has the same length as the plaintext).
 *
 * Header used with passphrases is extended from the header for master keys.
 * It contains additional data -- the KDF context -- storing KDF parameters.
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
 * |         KDF context length        |
 * +--------+--------+--------+--------+
 *
 *  Initialization Vector data (12 bytes)
 *                                     +--------+--------+--------+--------+
 *                                     |                                   |
 * +--------+--------+--------+--------+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 *
 *  Authentication Tag data (16 bytes)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 *
 *  Key derivation context data (22 bytes)
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+
 * |                                                                       |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+--------+--------+
 * |                                                     |
 * +--------+--------+--------+--------+--------+--------+
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
 * Currently Themis supports only one KDF -- PBKDF2 -- which has the following
 * context layout:
 *
 * ```
 *     0        1        2        3        4        5        6        7
 * +--------+--------+--------+--------+--------+--------+--------+--------+
 * |          iteration count          |   salt length   |                 |
 * +--------+--------+--------+--------+--------+--------+ - - - -+ - - - -+
 * |                       salt goes in every field                        |
 * + - - - -+ - - - -+ - - - -+ - - - -+ - - - -+ - - - -+--------+--------+
 * |                                                     |
 * +--------+--------+--------+--------+--------+--------+
 * ```
 *
 * @see Themis RFC 2 contains detailed reference on data formats.
 */

/**
 * Authentication token used by Seal and Token Protect modes with passphrases.
 *
 * This struct provides only API.
 * It does **not** represent actual in-memory data layout.
 */
struct themis_scell_auth_token_passphrase {
    uint32_t alg;
    const uint8_t* iv;
    uint32_t iv_length;
    const uint8_t* auth_tag;
    uint32_t auth_tag_length;
    uint32_t message_length;
    const uint8_t* kdf_context;
    uint32_t kdf_context_length;
};

static const uint64_t themis_scell_auth_token_passphrase_min_size = 5 * sizeof(uint32_t);

static inline uint64_t themis_scell_auth_token_passphrase_size(
    const struct themis_scell_auth_token_passphrase* hdr)
{
    uint64_t total_size = 0;
    /* Add separately to avoid overflows in intermediate calculations */
    total_size += sizeof(hdr->alg);
    total_size += sizeof(hdr->iv_length);
    total_size += hdr->iv_length;
    total_size += sizeof(hdr->auth_tag_length);
    total_size += hdr->auth_tag_length;
    total_size += sizeof(hdr->message_length);
    total_size += sizeof(hdr->kdf_context_length);
    total_size += hdr->kdf_context_length;
    return total_size;
}

/*
 * KDF context is not written to the buffer.
 * Call KDF-specific function to write the context into the buffer.
 */
static inline themis_status_t themis_write_scell_auth_token_passphrase(
    const struct themis_scell_auth_token_passphrase* hdr, uint8_t* buffer, size_t buffer_length)
{
    if (buffer_length < themis_scell_auth_token_passphrase_size(hdr)) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    buffer = stream_write_uint32LE(buffer, hdr->alg);
    buffer = stream_write_uint32LE(buffer, hdr->iv_length);
    buffer = stream_write_uint32LE(buffer, hdr->auth_tag_length);
    buffer = stream_write_uint32LE(buffer, hdr->message_length);
    buffer = stream_write_uint32LE(buffer, hdr->kdf_context_length);
    buffer = stream_write_bytes(buffer, hdr->iv, hdr->iv_length);
    buffer = stream_write_bytes(buffer, hdr->auth_tag, hdr->auth_tag_length);
    /* KDF context is written separately */
    return THEMIS_SUCCESS;
}

/*
 * KDF context buffer is retrieved into `hdr.kdf_context`, but you need to
 * parse it separately based on the KDF function used (see `alg` field).
 */
static inline themis_status_t themis_read_scell_auth_token_passphrase(
    const uint8_t* buffer, size_t buffer_length, struct themis_scell_auth_token_passphrase* hdr)
{
    uint64_t need_length = themis_scell_auth_token_passphrase_min_size;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_uint32LE(buffer, &hdr->alg);
    buffer = stream_read_uint32LE(buffer, &hdr->iv_length);
    buffer = stream_read_uint32LE(buffer, &hdr->auth_tag_length);
    buffer = stream_read_uint32LE(buffer, &hdr->message_length);
    buffer = stream_read_uint32LE(buffer, &hdr->kdf_context_length);
    /* Add separately to avoid overflows in intermediate calculations */
    need_length += hdr->iv_length;
    need_length += hdr->auth_tag_length;
    need_length += hdr->kdf_context_length;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_bytes(buffer, &hdr->iv, hdr->iv_length);
    buffer = stream_read_bytes(buffer, &hdr->auth_tag, hdr->auth_tag_length);
    buffer = stream_read_bytes(buffer, &hdr->kdf_context, hdr->kdf_context_length);
    return THEMIS_SUCCESS;
}

static inline themis_status_t themis_scell_auth_token_message_size(const uint8_t* auth_token,
                                                                   size_t auth_token_length,
                                                                   uint32_t* message_length)
{
    THEMIS_CHECK_PARAM(message_length != NULL);
    if (auth_token_length < themis_scell_auth_token_passphrase_min_size) {
        return THEMIS_FAIL;
    }
    const uint8_t* message_length_ptr = auth_token + 3 * sizeof(uint32_t);
    stream_read_uint32LE(message_length_ptr, message_length);
    return THEMIS_SUCCESS;
}

/**
 * KDF context used for PBKDF2 HMAC-SHA-256.
 *
 * This struct provides only API.
 * It does **not** represent actual in-memory data layout.
 */
struct themis_scell_pbkdf2_context {
    uint32_t iteration_count;
    const uint8_t* salt;
    uint16_t salt_length;
};

static const uint32_t themis_scell_pbkdf2_context_min_size = sizeof(uint32_t) + sizeof(uint16_t);

static inline uint32_t themis_scell_pbkdf2_context_size(const struct themis_scell_pbkdf2_context* ctx)
{
    uint32_t total_size = 0;
    /* Add separately to avoid overflows in intermediate calculations */
    total_size += sizeof(ctx->iteration_count);
    total_size += sizeof(ctx->salt_length);
    total_size += ctx->salt_length;
    return total_size;
}

static inline themis_status_t themis_write_scell_pbkdf2_context(
    const struct themis_scell_auth_token_passphrase* hdr,
    const struct themis_scell_pbkdf2_context* ctx,
    uint8_t* buffer,
    size_t buffer_length)
{
    if (buffer_length < themis_scell_auth_token_passphrase_size(hdr)) {
        return THEMIS_BUFFER_TOO_SMALL;
    }
    if (hdr->kdf_context_length != themis_scell_pbkdf2_context_size(ctx)) {
        return THEMIS_FAIL;
    }
    /* Add separately to avoid overflows in intermediate calculations */
    buffer += sizeof(hdr->alg);
    buffer += sizeof(hdr->iv_length);
    buffer += hdr->iv_length;
    buffer += sizeof(hdr->auth_tag_length);
    buffer += hdr->auth_tag_length;
    buffer += sizeof(hdr->message_length);
    buffer += sizeof(hdr->kdf_context_length);
    buffer = stream_write_uint32LE(buffer, ctx->iteration_count);
    buffer = stream_write_uint16LE(buffer, ctx->salt_length);
    buffer = stream_write_bytes(buffer, ctx->salt, ctx->salt_length);
    return THEMIS_SUCCESS;
}

static inline themis_status_t themis_read_scell_pbkdf2_context(
    const struct themis_scell_auth_token_passphrase* hdr, struct themis_scell_pbkdf2_context* ctx)
{
    const uint8_t* buffer = hdr->kdf_context;
    size_t buffer_length = hdr->kdf_context_length;
    size_t need_length = themis_scell_pbkdf2_context_min_size;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_uint32LE(buffer, &ctx->iteration_count);
    buffer = stream_read_uint16LE(buffer, &ctx->salt_length);
    need_length += ctx->salt_length;
    if (buffer_length < need_length) {
        return THEMIS_FAIL;
    }
    buffer = stream_read_bytes(buffer, &ctx->salt, ctx->salt_length);
    return THEMIS_SUCCESS;
}

themis_status_t themis_auth_sym_encrypt_message_with_passphrase(const uint8_t* passphrase,
                                                                size_t passphrase_length,
                                                                const uint8_t* message,
                                                                size_t message_length,
                                                                const uint8_t* user_context,
                                                                size_t user_context_length,
                                                                uint8_t* auth_token,
                                                                size_t* auth_token_length,
                                                                uint8_t* encrypted_message,
                                                                size_t* encrypted_message_length);

themis_status_t themis_auth_sym_decrypt_message_with_passphrase(const uint8_t* passphrase,
                                                                size_t passphrase_length,
                                                                const uint8_t* user_context,
                                                                size_t user_context_length,
                                                                const uint8_t* auth_token,
                                                                size_t auth_token_length,
                                                                const uint8_t* encrypted_message,
                                                                size_t encrypted_message_length,
                                                                uint8_t* message,
                                                                size_t* message_length);

#endif /* THEMIS_SECURE_CELL_SEAL_PASSPHRASE_H */
