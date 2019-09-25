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

#include "soter/soter_container.h"

#include <limits.h>

#include "soter/soter_crc32.h"
#include "soter/soter_portable_endian.h"

soter_status_t soter_update_container_checksum(soter_container_hdr_t* hdr)
{
    if (!hdr) {
        return SOTER_INVALID_PARAMETER;
    }

    hdr->crc = 0;
    hdr->crc = htobe32(soter_crc32(hdr, be32toh(hdr->size)));

    return SOTER_SUCCESS;
}

soter_status_t soter_verify_container_checksum(const soter_container_hdr_t* hdr)
{
    uint32_t dummy_crc = 0;
    soter_crc32_t crc;

    if (!hdr) {
        return SOTER_INVALID_PARAMETER;
    }

    crc = soter_crc32_create();
    soter_crc32_update(&crc, hdr, sizeof(soter_container_hdr_t) - sizeof(uint32_t));
    soter_crc32_update(&crc, &dummy_crc, sizeof(uint32_t));
    soter_crc32_update(&crc, hdr + 1, soter_container_data_size(hdr));

    if (hdr->crc == htobe32(soter_crc32_final(&crc))) {
        return SOTER_SUCCESS;
    }

    return SOTER_DATA_CORRUPT;
}

uint8_t* soter_container_data(soter_container_hdr_t* hdr)
{
    if (!hdr) {
        return NULL;
    }
    return (uint8_t*)(hdr + 1);
}

const uint8_t* soter_container_const_data(const soter_container_hdr_t* hdr)
{
    if (!hdr) {
        return NULL;
    }
    return (const uint8_t*)(hdr + 1);
}

size_t soter_container_data_size(const soter_container_hdr_t* hdr)
{
    size_t size = 0;
    if (!hdr) {
        return 0;
    }
    size = be32toh(hdr->size);
    if (size < sizeof(soter_container_hdr_t)) {
        return 0;
    }
    return size - sizeof(soter_container_hdr_t);
}

void soter_container_set_data_size(soter_container_hdr_t* hdr, size_t size)
{
    if (!hdr || size > UINT32_MAX - sizeof(soter_container_hdr_t)) {
        return;
    }
    hdr->size = htobe32(size + sizeof(soter_container_hdr_t));
}
