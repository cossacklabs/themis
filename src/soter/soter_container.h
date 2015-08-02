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

#include <stdint.h>
#include <soter/error.h>

#define SOTER_CONTAINER_TAG_LENGTH 4

struct soter_container_hdr_type
{
	char tag[SOTER_CONTAINER_TAG_LENGTH];
	int32_t size; /* Size is data + sizeof(soter_container_hdr_t), so should be not less than sizeof(soter_container_hdr_t). Network byte order. */
	uint32_t crc;
};

typedef struct soter_container_hdr_type soter_container_hdr_t;

soter_status_t soter_update_container_checksum(soter_container_hdr_t *hdr);
soter_status_t soter_verify_container_checksum(const soter_container_hdr_t *hdr);

#define soter_container_data(_HDR_) ((uint8_t *)((_HDR_) + 1))
#define soter_container_const_data(_HDR_) ((const uint8_t *)((_HDR_) + 1))
#define soter_container_data_size(_HDR_) ((size_t)ntohl((_HDR_)->size) - sizeof(soter_container_hdr_t))
#define soter_container_set_data_size(_HDR_, _SIZE_) ((_HDR_)->size = htonl(_SIZE_ + sizeof(soter_container_hdr_t)))

#endif /* SOTER_CONTAINER_H */
