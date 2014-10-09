/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_CONTAINER_H
#define SOTER_CONTAINER_H

#include <stdint.h>

#define SOTER_CONTAINER_TAG_LENGTH 4

struct soter_container_hdr_type
{
	char tag[SOTER_CONTAINER_TAG_LENGTH];
	int32_t size; /* Size is data + sizeof(soter_container_hdr_t), so should be not less than sizeof(soter_container_hdr_t) */
};

struct soter_container_hdr_type soter_container_hdr_t;

#endif /* SOTER_CONTAINER_H */
