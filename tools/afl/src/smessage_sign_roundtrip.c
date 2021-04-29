/*
 * Copyright (c) 2019 Cossack Labs Limited
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

    uint8_t* private_key_bytes = NULL;
    size_t private_key_size = 0;

    if (read_line_binary(input, &private_key_bytes, &private_key_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    uint8_t* public_key_bytes = NULL;
    size_t public_key_size = 0;

    if (read_line_binary(input, &public_key_bytes, &public_key_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    uint8_t* message_bytes = NULL;
    size_t message_size = 0;

    if (read_line_binary(input, &message_bytes, &message_size)) {
        fprintf(stderr, "failed to read %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    fclose(input);

    /*
     * Try signing the message.
     */

    uint8_t* signed_bytes = NULL;
    size_t signed_size = 0;

    err = themis_secure_message_sign(private_key_bytes,
                                     private_key_size,
                                     message_bytes,
                                     message_size,
                                     signed_bytes,
                                     &signed_size);

    if (err != THEMIS_BUFFER_TOO_SMALL) {
        fprintf(stderr, "failed to determine signed message size: %d\n", err);
        return 2;
    }

    signed_bytes = malloc(signed_size);
    if (!signed_bytes) {
        fprintf(stderr, "failed to allocate memory for signed message (%zu bytes)\n", signed_size);
        return 2;
    }

    err = themis_secure_message_sign(private_key_bytes,
                                     private_key_size,
                                     message_bytes,
                                     message_size,
                                     signed_bytes,
                                     &signed_size);

    if (err != THEMIS_SUCCESS) {
        fprintf(stderr, "failed to sign message: %d\n", err);
        return 2;
    }

    /*
     * Then try verifying the signature.
     */

    uint8_t* original_bytes = NULL;
    size_t original_size = 0;

    err = themis_secure_message_verify(public_key_bytes,
                                       public_key_size,
                                       signed_bytes,
                                       signed_size,
                                       original_bytes,
                                       &original_size);

    if (err != THEMIS_BUFFER_TOO_SMALL) {
        fprintf(stderr, "failed to determine original message size: %d\n", err);
        return 3;
    }

    original_bytes = malloc(original_size);
    if (!original_bytes) {
        fprintf(stderr, "failed to allocate memory for original message (%zu bytes)\n", original_size);
        return 3;
    }

    err = themis_secure_message_verify(public_key_bytes,
                                       public_key_size,
                                       signed_bytes,
                                       signed_size,
                                       original_bytes,
                                       &original_size);

    if (err != THEMIS_SUCCESS) {
        fprintf(stderr, "failed to verify message: %d\n", err);
        return 3;
    }

    /*
     * And finally check that the result is the same as the input.
     */

    if (original_size != message_size) {
        fprintf(stderr,
                "message length does not match: actual %zu, expected %zu\n",
                original_size,
                message_size);
        abort();
    }
    if (memcmp(message_bytes, original_bytes, message_size) != 0) {
        fprintf(stderr, "message content does not match\n");
        abort();
    }

    return 0;
}
