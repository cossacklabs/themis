/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef _SIGNED_MESSAGE_CONTAINER_H_
#define _SIGNED_MESSAGE_CONTAINER_H_

##include /themis/themis_container.h>

#define SIGNED_MASSAGE_TAG "SIGN"

struct themis_message_container_type{
  themis_container_header_t hdr;
  uint8_t* data;
};

struct signature_container_type{
  char signature_algorithm_tag[THEMIS_TAG_LENGTH];
  uint32_t length;
  uint32_t crc;
  uint8_t* data;
};

typedef struct message_container_type message_t;

struct signed_message_type{

  soter_container_hdr_t hdr;
  signature_t signature; 
  message_t message;
  
};

#endif /* _SIGNED_MESSAGE_CONTAINER_H_ */












