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

#ifndef SOTER_CONTAINER_H
#define SOTER_CONTAINER_H

#include <soter/soter_api.h>
#include <soter/soter_error.h>

#define SOTER_CONTAINER_TAG_LENGTH 4

#pragma pack(push, 1)
struct soter_container_hdr_type {
    char tag[SOTER_CONTAINER_TAG_LENGTH];
    uint32_t size; /* Size is data + sizeof(soter_container_hdr_t), so should be not less than
                      sizeof(soter_container_hdr_t). Network byte order. */
    uint32_t crc;
};
#pragma pack(pop)

typedef struct soter_container_hdr_type soter_container_hdr_t;

SOTER_API
soter_status_t soter_update_container_checksum(soter_container_hdr_t* hdr);

SOTER_API
soter_status_t soter_verify_container_checksum(const soter_container_hdr_t* hdr);

SOTER_API
uint8_t* soter_container_data(soter_container_hdr_t* hdr);

SOTER_API
const uint8_t* soter_container_const_data(const soter_container_hdr_t* hdr);

SOTER_API
size_t soter_container_data_size(const soter_container_hdr_t* hdr);

SOTER_API
void soter_container_set_data_size(soter_container_hdr_t* hdr, size_t size);

#endif /* SOTER_CONTAINER_H */
