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

#include "readline.h"

#include <stdlib.h>

int read_line_binary(FILE* input, uint8_t** out_bytes, size_t* out_size)
{
    uint8_t length_bytes[4] = {0};
    uint32_t length = 0;

    if (!input || !out_bytes || !out_size) {
        return -1;
    }

    *out_bytes = NULL;
    *out_size = 0;

    if (fread(length_bytes, 1, 4, input) != 4) {
        return -1;
    }

    length = (length << 8) | length_bytes[0];
    length = (length << 8) | length_bytes[1];
    length = (length << 8) | length_bytes[2];
    length = (length << 8) | length_bytes[3];

    *out_bytes = malloc(length);
    if (!*out_bytes) {
        return -1;
    }

    if (fread(*out_bytes, 1, length, input) != length) {
        free(*out_bytes);
        *out_bytes = NULL;
        return -1;
    }

    *out_size = length;

    return 0;
}
