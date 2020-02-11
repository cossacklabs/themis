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

#include "themis/sym_enc_message.h"

#include <string.h>

#include <soter/soter.h>

#include "themis/secure_cell_alg.h"
#include "themis/themis_portable_endian.h"

#define THEMIS_SYM_KDF_KEY_LABEL "Themis secure cell message key"
#define THEMIS_SYM_KDF_IV_LABEL "Themis secure cell message iv"

themis_status_t themis_sym_kdf(const uint8_t* master_key,
                               const size_t master_key_length,
                               const char* label,
                               const uint8_t* context,
                               const size_t context_length,
                               const uint8_t* context2,
                               const size_t context2_length,
                               uint8_t* key,
                               size_t key_length)
{
    THEMIS_CHECK_PARAM(master_key != NULL && master_key_length != 0);
    THEMIS_CHECK_PARAM(context != NULL && context_length != 0);
    soter_kdf_context_buf_t ctx[2] = {{context, context_length}, {context2, context2_length}};
    THEMIS_CHECK(soter_kdf(master_key,
                           master_key_length,
                           label,
                           ctx,
                           (context2 == NULL || context2_length == 0) ? 1 : 2,
                           key,
                           key_length)
                 == THEMIS_SUCCESS);
    return THEMIS_SUCCESS;
}

themis_status_t themis_auth_sym_plain_encrypt(uint32_t alg,
                                              const uint8_t* key,
                                              const size_t key_length,
                                              const uint8_t* iv,
                                              const size_t iv_length,
                                              const uint8_t* aad,
                                              const size_t aad_length,
                                              const uint8_t* message,
                                              const size_t message_length,
                                              uint8_t* encrypted_message,
                                              size_t* encrypted_message_length,
                                              uint8_t* auth_tag,
                                              uint32_t* auth_tag_length)
{
    size_t auth_tag_length_ = *auth_tag_length;
    soter_sym_ctx_t* ctx = soter_sym_aead_encrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
    THEMIS_CHECK(ctx != NULL);
    if (aad != NULL || aad_length != 0) {
        THEMIS_CHECK__(soter_sym_aead_encrypt_aad(ctx, aad, aad_length) == THEMIS_SUCCESS,
                       soter_sym_aead_encrypt_destroy(ctx);
                       return THEMIS_FAIL);
    }
    THEMIS_CHECK__(soter_sym_aead_encrypt_update(ctx, message, message_length, encrypted_message, encrypted_message_length)
                       == THEMIS_SUCCESS,
                   soter_sym_aead_encrypt_destroy(ctx);
                   return THEMIS_FAIL);
    THEMIS_CHECK__(soter_sym_aead_encrypt_final(ctx, auth_tag, &auth_tag_length_) == THEMIS_SUCCESS,
                   soter_sym_aead_encrypt_destroy(ctx);
                   return THEMIS_FAIL);
    soter_sym_aead_encrypt_destroy(ctx);
    if (auth_tag_length_ > UINT32_MAX) {
        return THEMIS_INVALID_PARAMETER;
    }
    *auth_tag_length = (uint32_t)auth_tag_length_;
    return THEMIS_SUCCESS;
}

themis_status_t themis_auth_sym_plain_decrypt(uint32_t alg,
                                              const uint8_t* key,
                                              const size_t key_length,
                                              const uint8_t* iv,
                                              const size_t iv_length,
                                              const uint8_t* aad,
                                              const size_t aad_length,
                                              const uint8_t* encrypted_message,
                                              const size_t encrypted_message_length,
                                              uint8_t* message,
                                              size_t* message_length,
                                              const uint8_t* auth_tag,
                                              const size_t auth_tag_length)
{
    soter_sym_ctx_t* ctx = soter_sym_aead_decrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
    THEMIS_CHECK(ctx != NULL)
    if (aad != NULL || aad_length != 0) {
        THEMIS_CHECK__(soter_sym_aead_decrypt_aad(ctx, aad, aad_length) == THEMIS_SUCCESS,
                       soter_sym_aead_decrypt_destroy(ctx);
                       return THEMIS_FAIL);
    }
    THEMIS_CHECK__(soter_sym_aead_decrypt_update(ctx, encrypted_message, encrypted_message_length, message, message_length)
                       == THEMIS_SUCCESS,
                   soter_sym_aead_decrypt_destroy(ctx);
                   return THEMIS_FAIL);
    THEMIS_CHECK__(soter_sym_aead_decrypt_final(ctx, auth_tag, auth_tag_length) == THEMIS_SUCCESS,
                   soter_sym_aead_decrypt_destroy(ctx);
                   return THEMIS_FAIL);
    soter_sym_aead_decrypt_destroy(ctx);
    return THEMIS_SUCCESS;
}

themis_status_t themis_sym_plain_encrypt(uint32_t alg,
                                         const uint8_t* key,
                                         const size_t key_length,
                                         const uint8_t* iv,
                                         const size_t iv_length,
                                         const uint8_t* message,
                                         const size_t message_length,
                                         uint8_t* encrypted_message,
                                         size_t* encrypted_message_length)
{
    soter_sym_ctx_t* ctx = soter_sym_encrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
    THEMIS_CHECK__(ctx, return THEMIS_NO_MEMORY);
    size_t add_length = (*encrypted_message_length);
    themis_status_t res = soter_sym_encrypt_update(ctx,
                                                   message,
                                                   message_length,
                                                   encrypted_message,
                                                   encrypted_message_length);
    THEMIS_CHECK__(THEMIS_SUCCESS == res, soter_sym_encrypt_destroy(ctx); return res);
    add_length -= (*encrypted_message_length);
    res = soter_sym_encrypt_final(ctx, encrypted_message + (*encrypted_message_length), &add_length);
    THEMIS_CHECK__(res == THEMIS_SUCCESS, soter_sym_encrypt_destroy(ctx);
                   (*encrypted_message_length) += add_length;
                   return res);
    (*encrypted_message_length) += add_length;
    soter_sym_encrypt_destroy(ctx);
    return THEMIS_SUCCESS;
}

themis_status_t themis_sym_plain_decrypt(uint32_t alg,
                                         const uint8_t* key,
                                         const size_t key_length,
                                         const uint8_t* iv,
                                         const size_t iv_length,
                                         const uint8_t* encrypted_message,
                                         const size_t encrypted_message_length,
                                         uint8_t* message,
                                         size_t* message_length)
{
    soter_sym_ctx_t* ctx = soter_sym_decrypt_create(alg, key, key_length, NULL, 0, iv, iv_length);
    THEMIS_CHECK(ctx != NULL);
    size_t add_length = (*message_length);
    themis_status_t res = soter_sym_decrypt_update(ctx,
                                                   encrypted_message,
                                                   encrypted_message_length,
                                                   message,
                                                   message_length);
    THEMIS_CHECK__(THEMIS_SUCCESS == res, soter_sym_decrypt_destroy(ctx); return res);
    add_length -= (*message_length);
    res = soter_sym_decrypt_final(ctx, message + (*message_length), &add_length);
    THEMIS_CHECK__(res == THEMIS_SUCCESS, soter_sym_decrypt_destroy(ctx);
                   (*message_length) += add_length;
                   return res);
    (*message_length) += add_length;
    soter_sym_decrypt_destroy(ctx);
    return THEMIS_SUCCESS;
}

static inline size_t default_auth_token_size(void)
{
    return themis_scell_auth_token_key_min_size + THEMIS_AUTH_SYM_IV_LENGTH
           + THEMIS_AUTH_SYM_AUTH_TAG_LENGTH;
}

static themis_status_t themis_auth_sym_derive_encryption_key(const struct themis_scell_auth_token_key* hdr,
                                                             const uint8_t* key,
                                                             size_t key_length,
                                                             const uint8_t* user_context,
                                                             size_t user_context_length,
                                                             uint8_t* derived_key,
                                                             size_t* derived_key_length)
{
    size_t required_length = soter_alg_key_length(hdr->alg);
    /* Internal buffer must have suitable size */
    if (*derived_key_length < required_length) {
        return THEMIS_FAIL;
    }
    *derived_key_length = required_length;
    /*
     * SOTER_SYM_NOKDF means Soter KDF in this context.
     * This is the only KDF allowed for master key API.
     */
    switch (soter_alg_kdf(hdr->alg)) {
    case SOTER_SYM_NOKDF: {
        uint8_t message_length_label[sizeof(uint32_t)] = {0};
        stream_write_uint32LE(message_length_label, hdr->message_length);
        return themis_sym_kdf(key,
                              key_length,
                              THEMIS_SYM_KDF_KEY_LABEL,
                              message_length_label,
                              sizeof(message_length_label),
                              user_context,
                              user_context_length,
                              derived_key,
                              *derived_key_length);
    }
    default:
        return THEMIS_FAIL;
    }
}

#ifdef SCELL_COMPAT
static themis_status_t themis_auth_sym_derive_encryption_key_compat(
    const struct themis_scell_auth_token_key* hdr,
    const uint8_t* key,
    size_t key_length,
    const uint8_t* user_context,
    size_t user_context_length,
    uint8_t* derived_key,
    size_t* derived_key_length)
{
    size_t required_length = soter_alg_key_length(hdr->alg);
    /* Internal buffer must have suitable size */
    if (*derived_key_length < required_length) {
        return THEMIS_FAIL;
    }
    *derived_key_length = required_length;
    /*
     * SOTER_SYM_NOKDF means Soter KDF in this context.
     * This is the only KDF allowed for master key API.
     */
    switch (soter_alg_kdf(hdr->alg)) {
    case SOTER_SYM_NOKDF: {
        /*
         * Themis 0.9.6 incorrectly used 64-bit size_t (as uint64_) for this field.
         */
        uint8_t message_length_label[sizeof(uint64_t)] = {0};
        stream_write_uint64LE(message_length_label, hdr->message_length);
        return themis_sym_kdf(key,
                              key_length,
                              THEMIS_SYM_KDF_KEY_LABEL,
                              message_length_label,
                              sizeof(message_length_label),
                              user_context,
                              user_context_length,
                              derived_key,
                              *derived_key_length);
    }
    default:
        return THEMIS_FAIL;
    }
}
#endif

themis_status_t themis_auth_sym_encrypt_message_(const uint8_t* key,
                                                 size_t key_length,
                                                 const uint8_t* message,
                                                 size_t message_length,
                                                 const uint8_t* user_context,
                                                 size_t user_context_length,
                                                 uint8_t* auth_token,
                                                 size_t* auth_token_length,
                                                 uint8_t* encrypted_message,
                                                 size_t* encrypted_message_length)
{
    themis_status_t res = THEMIS_FAIL;
    uint8_t iv[THEMIS_AUTH_SYM_IV_LENGTH] = {0};
    uint8_t auth_tag[THEMIS_AUTH_SYM_AUTH_TAG_LENGTH] = {0};
    uint8_t derived_key[THEMIS_AUTH_SYM_KEY_LENGTH / 8] = {0};
    size_t derived_key_length = sizeof(derived_key);
    struct themis_scell_auth_token_key hdr = {0};

    /* Message length is currently stored as 32-bit integer, sorry */
    if (message_length > UINT32_MAX) {
        return THEMIS_INVALID_PARAMETER;
    }

    hdr.alg = THEMIS_AUTH_SYM_ALG;
    hdr.iv = iv;
    hdr.iv_length = sizeof(iv);
    hdr.auth_tag = auth_tag;
    hdr.auth_tag_length = sizeof(auth_tag);
    hdr.message_length = (uint32_t)message_length;

    res = themis_auth_sym_derive_encryption_key(&hdr,
                                                key,
                                                key_length,
                                                user_context,
                                                user_context_length,
                                                derived_key,
                                                &derived_key_length);
    if (res != THEMIS_SUCCESS) {
        goto error;
    }

    res = soter_rand(iv, sizeof(iv));
    if (res != THEMIS_SUCCESS) {
        goto error;
    }

    res = themis_auth_sym_plain_encrypt(hdr.alg,
                                        derived_key,
                                        derived_key_length,
                                        hdr.iv,
                                        hdr.iv_length,
                                        user_context,
                                        user_context_length,
                                        message,
                                        message_length,
                                        encrypted_message,
                                        encrypted_message_length,
                                        &auth_tag[0],
                                        &hdr.auth_tag_length);
    if (res != THEMIS_SUCCESS) {
        goto error;
    }

    if (*auth_token_length < themis_scell_auth_token_key_size(&hdr)) {
        *auth_token_length = themis_scell_auth_token_key_size(&hdr);
        res = THEMIS_BUFFER_TOO_SMALL;
        goto error;
    }
    res = themis_write_scell_auth_token_key(&hdr, auth_token, *auth_token_length);
    if (res != THEMIS_SUCCESS) {
        goto error;
    }
    *auth_token_length = themis_scell_auth_token_key_size(&hdr);
    *encrypted_message_length = message_length;

error:
    soter_wipe(iv, sizeof(iv));
    soter_wipe(auth_tag, sizeof(auth_tag));
    soter_wipe(derived_key, sizeof(derived_key));

    return res;
}

themis_status_t themis_auth_sym_encrypt_message(const uint8_t* key,
                                                size_t key_length,
                                                const uint8_t* message,
                                                size_t message_length,
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                uint8_t* auth_token,
                                                size_t* auth_token_length,
                                                uint8_t* encrypted_message,
                                                size_t* encrypted_message_length)
{
    THEMIS_CHECK_PARAM(key != NULL && key_length != 0);
    THEMIS_CHECK_PARAM(message != NULL && message_length != 0);
    if (user_context_length != 0) {
        THEMIS_CHECK_PARAM(user_context != NULL);
    }
    THEMIS_CHECK_PARAM(auth_token_length != NULL);
    THEMIS_CHECK_PARAM(encrypted_message_length != NULL);

    if (!auth_token_length || !encrypted_message || *auth_token_length < default_auth_token_size()
        || *encrypted_message_length < message_length) {
        *auth_token_length = default_auth_token_size();
        *encrypted_message_length = message_length;
        return THEMIS_BUFFER_TOO_SMALL;
    }

    return themis_auth_sym_encrypt_message_(key,
                                            key_length,
                                            message,
                                            message_length,
                                            user_context,
                                            user_context_length,
                                            auth_token,
                                            auth_token_length,
                                            encrypted_message,
                                            encrypted_message_length);
}

themis_status_t themis_auth_sym_decrypt_message_(const uint8_t* key,
                                                 size_t key_length,
                                                 const uint8_t* user_context,
                                                 size_t user_context_length,
                                                 const uint8_t* auth_token,
                                                 size_t auth_token_length,
                                                 const uint8_t* encrypted_message,
                                                 const size_t encrypted_message_length,
                                                 uint8_t* message,
                                                 size_t* message_length)
{
    themis_status_t res = THEMIS_FAIL;
    struct themis_scell_auth_token_key hdr = {0};
    /* Use maximum possible length, not the default one */
    uint8_t derived_key[THEMIS_AUTH_SYM_MAX_KEY_LENGTH / 8] = {0};
    size_t derived_key_length = sizeof(derived_key);

    res = themis_read_scell_auth_token_key(auth_token, auth_token_length, &hdr);
    if (res != THEMIS_SUCCESS) {
        return res;
    }

    /* Check that message header is consistent with our expectations */
    if (hdr.message_length != encrypted_message_length) {
        return THEMIS_FAIL;
    }
    if (!soter_alg_reserved_bits_valid(hdr.alg)) {
        return THEMIS_FAIL;
    }

    res = themis_auth_sym_derive_encryption_key(&hdr,
                                                key,
                                                key_length,
                                                user_context,
                                                user_context_length,
                                                derived_key,
                                                &derived_key_length);
    if (res != THEMIS_SUCCESS) {
        goto error;
    }

    res = themis_auth_sym_plain_decrypt(hdr.alg,
                                        derived_key,
                                        derived_key_length,
                                        hdr.iv,
                                        hdr.iv_length,
                                        user_context,
                                        user_context_length,
                                        encrypted_message,
                                        encrypted_message_length,
                                        message,
                                        message_length,
                                        hdr.auth_tag,
                                        hdr.auth_tag_length);
    /*
     * Themis 0.9.6 used slightly different KDF. If decryption fails,
     * maybe it was encrypted with that incorrect key. Try it out.
     */
#ifdef SCELL_COMPAT
    if (res != THEMIS_SUCCESS && res != THEMIS_BUFFER_TOO_SMALL && sizeof(size_t) == sizeof(uint64_t)) {
        res = themis_auth_sym_derive_encryption_key_compat(&hdr,
                                                           key,
                                                           key_length,
                                                           user_context,
                                                           user_context_length,
                                                           derived_key,
                                                           &derived_key_length);
        if (res != THEMIS_SUCCESS) {
            goto error;
        }
        res = themis_auth_sym_plain_decrypt(hdr.alg,
                                            derived_key,
                                            derived_key_length,
                                            hdr.iv,
                                            hdr.iv_length,
                                            user_context,
                                            user_context_length,
                                            encrypted_message,
                                            encrypted_message_length,
                                            message,
                                            message_length,
                                            hdr.auth_tag,
                                            hdr.auth_tag_length);
    }
#endif

    /* Sanity check of resulting message length */
    if (*message_length != encrypted_message_length) {
        res = THEMIS_FAIL;
        goto error;
    }

error:
    soter_wipe(derived_key, sizeof(derived_key));

    return res;
}

themis_status_t themis_auth_sym_decrypt_message(const uint8_t* key,
                                                size_t key_length,
                                                const uint8_t* user_context,
                                                size_t user_context_length,
                                                const uint8_t* auth_token,
                                                size_t auth_token_length,
                                                const uint8_t* encrypted_message,
                                                size_t encrypted_message_length,
                                                uint8_t* message,
                                                size_t* message_length)
{
    themis_status_t res = THEMIS_FAIL;
    uint32_t expected_message_length = 0;

    THEMIS_CHECK_PARAM(key != NULL && key_length != 0);
    if (user_context_length != 0) {
        THEMIS_CHECK_PARAM(user_context != NULL);
    }
    THEMIS_CHECK_PARAM(auth_token != NULL && auth_token_length != 0);
    THEMIS_CHECK_PARAM(message_length != NULL);

    /* Do a quick guess without parsing the message too deeply here */
    res = themis_scell_auth_token_key_message_size(auth_token, auth_token_length, &expected_message_length);
    if (res != THEMIS_SUCCESS) {
        return res;
    }
    if (!message || *message_length < expected_message_length) {
        *message_length = expected_message_length;
        return THEMIS_BUFFER_TOO_SMALL;
    }

    /* encrypted_message may be omitted when only querying plaintext size */
    THEMIS_CHECK_PARAM(encrypted_message != NULL && encrypted_message_length != 0);

    return themis_auth_sym_decrypt_message_(key,
                                            key_length,
                                            user_context,
                                            user_context_length,
                                            auth_token,
                                            auth_token_length,
                                            encrypted_message,
                                            encrypted_message_length,
                                            message,
                                            message_length);
}

typedef struct themis_sym_message_hdr_type {
    uint32_t alg;
    uint32_t iv_length;
    uint32_t message_length;
} themis_sym_message_hdr_t;

themis_status_t themis_sym_encrypt_message_u_(const uint8_t* key,
                                              const size_t key_length,
                                              const uint8_t* message,
                                              const size_t message_length,
                                              const uint8_t* context,
                                              const size_t context_length,
                                              uint8_t* encrypted_message,
                                              size_t* encrypted_message_length)
{
    THEMIS_CHECK_PARAM(message != NULL && message_length != 0);
    THEMIS_CHECK_PARAM(context != NULL && context_length != 0);
    if ((*encrypted_message_length) < message_length) {
        (*encrypted_message_length) = message_length;
        return THEMIS_BUFFER_TOO_SMALL;
    }
    (*encrypted_message_length) = message_length;
    uint8_t iv[THEMIS_SYM_IV_LENGTH];
    THEMIS_STATUS_CHECK(themis_sym_kdf(key,
                                       key_length,
                                       THEMIS_SYM_KDF_IV_LABEL,
                                       context,
                                       context_length,
                                       NULL,
                                       0,
                                       iv,
                                       THEMIS_SYM_IV_LENGTH),
                        THEMIS_SUCCESS);
    THEMIS_STATUS_CHECK(themis_sym_plain_encrypt(THEMIS_SYM_ALG,
                                                 key,
                                                 key_length,
                                                 iv,
                                                 THEMIS_SYM_IV_LENGTH,
                                                 message,
                                                 message_length,
                                                 encrypted_message,
                                                 encrypted_message_length),
                        THEMIS_SUCCESS);
    return THEMIS_SUCCESS;
}

themis_status_t themis_sym_encrypt_message_u(const uint8_t* key,
                                             const size_t key_length,
                                             const uint8_t* context,
                                             const size_t context_length,
                                             const uint8_t* message,
                                             const size_t message_length,
                                             uint8_t* encrypted_message,
                                             size_t* encrypted_message_length)
{
    uint8_t key_[THEMIS_SYM_KEY_LENGTH / 8];

    // TODO: TYPE WARNING Should update `sizeof(uint32_t)` to `sizeof(message_length)` after
    // changing encrypted_message_length type to uint32_t
    THEMIS_STATUS_CHECK(themis_sym_kdf(key,
                                       key_length,
                                       THEMIS_SYM_KDF_KEY_LABEL,
                                       (uint8_t*)(&message_length),
                                       sizeof(uint32_t),
                                       NULL,
                                       0,
                                       key_,
                                       sizeof(key_)),
                        THEMIS_SUCCESS);
    return themis_sym_encrypt_message_u_(key_,
                                         sizeof(key_),
                                         message,
                                         message_length,
                                         context,
                                         context_length,
                                         encrypted_message,
                                         encrypted_message_length);
}

themis_status_t themis_sym_decrypt_message_u_(const uint8_t* key,
                                              const size_t key_length,
                                              const uint8_t* context,
                                              const size_t context_length,
                                              const uint8_t* encrypted_message,
                                              const size_t encrypted_message_length,
                                              uint8_t* message,
                                              size_t* message_length)
{
    THEMIS_CHECK_PARAM(encrypted_message != NULL && encrypted_message_length != 0);
    THEMIS_CHECK_PARAM(context != NULL && context_length != 0);
    if ((*message_length) < encrypted_message_length) {
        (*message_length) = encrypted_message_length;
        return THEMIS_BUFFER_TOO_SMALL;
    }
    (*message_length) = encrypted_message_length;
    uint8_t iv[THEMIS_SYM_IV_LENGTH];
    THEMIS_STATUS_CHECK(themis_sym_kdf(key,
                                       key_length,
                                       THEMIS_SYM_KDF_IV_LABEL,
                                       context,
                                       context_length,
                                       NULL,
                                       0,
                                       iv,
                                       THEMIS_SYM_IV_LENGTH),
                        THEMIS_SUCCESS);
    THEMIS_STATUS_CHECK(themis_sym_plain_decrypt(THEMIS_SYM_ALG,
                                                 key,
                                                 key_length,
                                                 iv,
                                                 THEMIS_SYM_IV_LENGTH,
                                                 encrypted_message,
                                                 encrypted_message_length,
                                                 message,
                                                 message_length),
                        THEMIS_SUCCESS);
    return THEMIS_SUCCESS;
}

themis_status_t themis_sym_decrypt_message_u(const uint8_t* key,
                                             const size_t key_length,
                                             const uint8_t* context,
                                             const size_t context_length,
                                             const uint8_t* encrypted_message,
                                             const size_t encrypted_message_length,
                                             uint8_t* message,
                                             size_t* message_length)
{
    uint8_t key_[THEMIS_SYM_KEY_LENGTH / 8];

    // TODO: TYPE WARNING Should update `sizeof(uint32_t)` to `sizeof(encrypted_message_length)`
    // after changing encrypted_message_length type to uint32_t
    THEMIS_STATUS_CHECK(themis_sym_kdf(key,
                                       key_length,
                                       THEMIS_SYM_KDF_KEY_LABEL,
                                       (uint8_t*)(&encrypted_message_length),
                                       sizeof(uint32_t),
                                       NULL,
                                       0,
                                       key_,
                                       sizeof(key_)),
                        THEMIS_SUCCESS);
    themis_status_t decryption_result = themis_sym_decrypt_message_u_(key_,
                                                                      sizeof(key_),
                                                                      context,
                                                                      context_length,
                                                                      encrypted_message,
                                                                      encrypted_message_length,
                                                                      message,
                                                                      message_length);

#ifdef SCELL_COMPAT

    // we are on x64, should sizeof(uin64_t) for backwards compatibility with themis 0.9.6 x64
    if (sizeof(size_t) == sizeof(uint64_t)) {
        if (decryption_result != THEMIS_SUCCESS && decryption_result != THEMIS_BUFFER_TOO_SMALL) {
            // TODO: TYPE WARNING `sizeof(uint64_t)`. Fix that on next versions
            THEMIS_STATUS_CHECK(themis_sym_kdf(key,
                                               key_length,
                                               THEMIS_SYM_KDF_KEY_LABEL,
                                               (uint8_t*)(&encrypted_message_length),
                                               sizeof(uint64_t),
                                               NULL,
                                               0,
                                               key_,
                                               sizeof(key_)),
                                THEMIS_SUCCESS);
            decryption_result = themis_sym_decrypt_message_u_(key_,
                                                              sizeof(key_),
                                                              context,
                                                              context_length,
                                                              encrypted_message,
                                                              encrypted_message_length,
                                                              message,
                                                              message_length);
        }
    }
#endif

    return decryption_result;
}
