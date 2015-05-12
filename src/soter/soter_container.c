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

#include <soter/soter_container.h>
#include <soter/soter.h>
#include <soter/error.h>
#include <soter/soter_crc32.h>
#include <arpa/inet.h>
soter_status_t soter_update_container_checksum(soter_container_hdr_t *hdr)
{
	hdr->crc = 0;
	hdr->crc = htonl(soter_crc32(hdr, ntohl(hdr->size)));

	return SOTER_SUCCESS;
}

soter_status_t soter_verify_container_checksum(const soter_container_hdr_t *hdr)
{
	uint32_t dummy_crc = 0;
	soter_crc32_t crc = soter_crc32_create();

	soter_crc32_update(&crc, hdr, sizeof(soter_container_hdr_t) - sizeof(uint32_t));
	soter_crc32_update(&crc, &dummy_crc, sizeof(uint32_t));
	soter_crc32_update(&crc, hdr + 1, ntohl(hdr->size) - sizeof(soter_container_hdr_t));

	if (hdr->crc == htonl(soter_crc32_final(&crc)))
	{
		return SOTER_SUCCESS;
	}
	else
	{
		return SOTER_DATA_CORRUPT;
	}
}
