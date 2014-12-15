/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_CONTAINER_H
#define SOTER_CONTAINER_H

#include <stdint.h>
#include <soter/soter.h>

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
