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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <themis/themis.h>

#include "readline.h"

int main(int argc, char** argv)
{
    themis_status_t err = THEMIS_SUCCESS;

    /*
     * Read test data.
     */

    if (argc != 2) {
        fprintf(stderr, "usage:\n\t%s <input-file>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "rb");
    if (!input) {
        fprintf(stderr, "failed to open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    uint32_t passphrase_size = 0;
    uint32_t user_context_size = 0;
    uint32_t message_size = 0;

    if (read_u32be(input, &passphrase_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    if (read_u32be(input, &user_context_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    if (read_u32be(input, &message_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    fclose(input);

    /*
     * 32-bit systems normally do not support more than 2 GB of virtual memory
     * and allocations for more than 2 GB are always failing. However, builds
     * with enabled Address Sanitizer on 32-bit systems usually abort when
     * trying to allocate even modest 100 MB. Avoid such false positives.
     */

    if (sizeof(size_t) == sizeof(uint32_t)) {
        bool insane_passphrase = (passphrase_size > MAX_SANE_LENGTH);
        bool insane_context = (user_context_size > MAX_SANE_LENGTH);
        bool insane_message = (message_size > MAX_SANE_LENGTH);
        if (insane_passphrase || insane_context || insane_message) {
            fprintf(stderr, "cannot allocate too much on 32-bit system\n");
            return 1;
        }
    }

    /*
     * Allocate memory. We're not particuarly interested in content of those
     * messages so use all zeros (predicatable and forces allocation).
     */

    uint8_t* passphrase_bytes = NULL;
    uint8_t* user_context_bytes = NULL;
    uint8_t* message_bytes = NULL;

    passphrase_bytes = calloc(1, passphrase_size);
    if (!passphrase_bytes) {
        fprintf(stderr, "failed to allocate %u bytes\n", passphrase_size);
        return 1;
    }
    user_context_bytes = calloc(1, user_context_size);
    if (!user_context_bytes) {
        fprintf(stderr, "failed to allocate %u bytes\n", user_context_size);
        return 1;
    }
    message_bytes = calloc(1, message_size);
    if (!message_bytes) {
        fprintf(stderr, "failed to allocate %u bytes\n", message_size);
        return 1;
    }

    /*
     * Try encrypting the message.
     */

    uint8_t* encrypted_bytes = NULL;
    size_t encrypted_size = 0;

    err = themis_secure_cell_encrypt_seal_with_passphrase(passphrase_bytes,
                                                          passphrase_size,
                                                          user_context_bytes,
                                                          user_context_size,
                                                          message_bytes,
                                                          message_size,
                                                          NULL,
                                                          &encrypted_size);

    if (err != THEMIS_BUFFER_TOO_SMALL) {
        fprintf(stderr, "failed to determine encrypted message size: %d\n", err);
        return 2;
    }

    encrypted_bytes = malloc(encrypted_size);
    if (!encrypted_bytes) {
        fprintf(stderr, "failed to allocate memory for encrypted message (%zu bytes)\n", encrypted_size);
        return 2;
    }

    err = themis_secure_cell_encrypt_seal_with_passphrase(passphrase_bytes,
                                                          passphrase_size,
                                                          user_context_bytes,
                                                          user_context_size,
                                                          message_bytes,
                                                          message_size,
                                                          encrypted_bytes,
                                                          &encrypted_size);

    if (err != THEMIS_SUCCESS) {
        fprintf(stderr, "failed to encrypt message: %d\n", err);
        return 2;
    }

    /*
     * Then try decrypting it back.
     */

    uint8_t* decrypted_bytes = NULL;
    size_t decrypted_size = 0;

    err = themis_secure_cell_decrypt_seal_with_passphrase(passphrase_bytes,
                                                          passphrase_size,
                                                          user_context_bytes,
                                                          user_context_size,
                                                          encrypted_bytes,
                                                          encrypted_size,
                                                          NULL,
                                                          &decrypted_size);

    if (err != THEMIS_BUFFER_TOO_SMALL) {
        fprintf(stderr, "failed to determine decrypted message size: %d\n", err);
        return 3;
    }

    decrypted_bytes = malloc(decrypted_size);
    if (!decrypted_bytes) {
        fprintf(stderr, "failed to allocate memory for decrypted message (%zu bytes)\n", decrypted_size);
        return 3;
    }

    err = themis_secure_cell_decrypt_seal_with_passphrase(passphrase_bytes,
                                                          passphrase_size,
                                                          user_context_bytes,
                                                          user_context_size,
                                                          encrypted_bytes,
                                                          encrypted_size,
                                                          decrypted_bytes,
                                                          &decrypted_size);

    if (err != THEMIS_SUCCESS) {
        fprintf(stderr, "failed to decrypt message: %d\n", err);
        return 3;
    }

    /*
     * And finally check that the result is the same as the input.
     */

    if (decrypted_size != message_size) {
        fprintf(stderr,
                "message length does not match: actual %zu, expected %u\n",
                decrypted_size,
                message_size);
        abort();
    }
    if (memcmp(message_bytes, decrypted_bytes, message_size) != 0) {
        fprintf(stderr, "message content does not match\n");
        abort();
    }

    return 0;
}
