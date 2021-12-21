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
     * Try decrypting it.
     */

    uint8_t* decrypted_bytes = NULL;
    size_t decrypted_size = 0;

    err = themis_secure_message_decrypt(private_key_bytes,
                                        private_key_size,
                                        public_key_bytes,
                                        public_key_size,
                                        message_bytes,
                                        message_size,
                                        NULL,
                                        &decrypted_size);

    if (err != THEMIS_BUFFER_TOO_SMALL) {
        fprintf(stderr, "failed to determine decrypted message size: %d\n", err);
        return 2;
    }

    decrypted_bytes = malloc(decrypted_size);
    if (!decrypted_bytes) {
        fprintf(stderr, "failed to allocate memory for decrypted message (%zu bytes)\n", decrypted_size);
        return 2;
    }

    err = themis_secure_message_decrypt(private_key_bytes,
                                        private_key_size,
                                        public_key_bytes,
                                        public_key_size,
                                        message_bytes,
                                        message_size,
                                        decrypted_bytes,
                                        &decrypted_size);

    if (err != THEMIS_SUCCESS) {
        fprintf(stderr, "failed to decrypt message: %d\n", err);
        return 2;
    }

    return 0;
}
