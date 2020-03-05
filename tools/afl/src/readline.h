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

#ifndef READ_LINE_H
#define READ_LINE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_SANE_LENGTH (50 * 1024 * 1024)

/**
 * Read a byte string for a file, prefixed with 4-byte length (big-endian).
 *
 * @param [in]  input      file to read, must be non-NULL
 * @param [out] out_bytes  pointer to resulting bytes will be put here,
 *                         must be non-NULL, free memory with free()
 * @param [out] out_size   length of the data will be put here,
 *                         must be non-NULL
 *
 * @returns zero on success, negative value on error.
 */
int read_line_binary(FILE* input, uint8_t** out_bytes, size_t* out_size);

/**
 * Read a big-endian unsinged 32-bit integer for a file.
 *
 * @param [in]  input      file to read, must be non-NULL
 * @param [out] out_value  resulting host-endian value will be put here,
 *                         must be non-NULL
 *
 * @returns zero on success, negative value on error.
 */
int read_u32be(FILE* input, uint32_t* out_value);

#endif /* READ_LINE_H */
