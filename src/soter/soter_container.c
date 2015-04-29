/**
 * @file
 *
 * (c) CossackLabs
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
