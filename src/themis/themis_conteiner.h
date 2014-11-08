/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _THEMIS_CONTEINER_H_
#define _THEMIS_CONTEINER_H_

#include "themis.h"

#define THEMIS_TAG_LENGTH 4

struct themis_container_header_type{
  char tag[THEMIS_TAG_LENGTH];
  uint32_t length;
  uint32_t crc;
};

typedef struct themis_container_header_type themis_container_header_t;

struct themis_container_type{
  themis_container_header_t hdr;
  uint8_t* data;
};

typedef struct themis_container_type themis_container_t;

themis_container_t* themis_container_create_new(const uint32_t tag);
themis_container_t* themis_container_create_from_buffer(const uint8_t* buffer, const size_t buffer_length);
themis_status_t themis_container_set_data(themis_container_t* ctr, const uint8_t* data, const size_t data_length);
themis_status_t themis_container_to_buffer(const themis_container_t* ctr, uint8_t* buffer, size_t* buffer_length);
themis_status_t themis_container_write(const themis_container_t* ctr, FILE* stream);
themis_status_t themis_container_destroy(themis_container_t* ctr);

#endif /* _THEMIS_CONTEINER_H_ */
