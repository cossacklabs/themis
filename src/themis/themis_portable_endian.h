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

#ifndef THEMIS_PORTABLE_ENDIAN_H
#define THEMIS_PORTABLE_ENDIAN_H

#include <stdint.h>
#include <string.h>

#include <soter/soter_portable_endian.h>

static inline uint8_t* stream_write_uint64LE(uint8_t* buffer, uint64_t value)
{
    uint64_t encoded = htole64(value);
    memmove(buffer, &encoded, sizeof(encoded));
    return buffer + sizeof(encoded);
}

static inline uint8_t* stream_write_uint32LE(uint8_t* buffer, uint32_t value)
{
    uint32_t encoded = htole32(value);
    memmove(buffer, &encoded, sizeof(encoded));
    return buffer + sizeof(encoded);
}

static inline const uint8_t* stream_read_uint32LE(const uint8_t* buffer, uint32_t* value)
{
    memmove(value, buffer, sizeof(*value));
    *value = le32toh(*value);
    return buffer + sizeof(*value);
}

static inline uint8_t* stream_write_uint16LE(uint8_t* buffer, uint16_t value)
{
    uint16_t encoded = htole16(value);
    memmove(buffer, &encoded, sizeof(encoded));
    return buffer + sizeof(encoded);
}

static inline const uint8_t* stream_read_uint16LE(const uint8_t* buffer, uint16_t* value)
{
    memmove(value, buffer, sizeof(*value));
    *value = le16toh(*value);
    return buffer + sizeof(*value);
}

static inline uint8_t* stream_write_bytes(uint8_t* buffer, const uint8_t* bytes, size_t length)
{
    memmove(buffer, bytes, length);
    return buffer + length;
}

static inline const uint8_t* stream_read_bytes(const uint8_t* buffer, const uint8_t** bytes, size_t length)
{
    *bytes = buffer;
    return buffer + length;
}

#endif /* THEMIS_PORTABLE_ENDIAN_H */
