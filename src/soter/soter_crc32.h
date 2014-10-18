/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_CRC32_H
#define SOTER_CRC32_H

#include <soter/soter.h>

typedef uint32_t soter_crc32_t;

uint32_t soter_crc32c(const uint8_t *buffer, size_t length);

soter_crc32_t soter_crc32_create(void);
void soter_crc32_update(soter_crc32_t *crc, const void *buf, size_t len);
uint32_t soter_crc32_final(soter_crc32_t *crc);

uint32_t soter_crc32(const void *buf, size_t len);

#endif /* SOTER_CRC32_H */
