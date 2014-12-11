/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS_MESSAGE_H_
#define THEMIS_MESSAGE_H_

#include "common/error.h"
#include "themis/themis.h"

struct themis_message_type{
  size_t length;
  uint8_t* data;
};

typedef struct themis_message_type themis_message_t;

themis_message_t* themis_message_init();
themis_status_t themis_message_set(themis_message_t* ctx, const uint8_t* message, const size_t message_length);

const uint8_t* themis_message_get_data(themis_message_t* ctx);
size_t themis_message_get_length(themis_message_t* ctx);

themis_status_t themis_message_destroy(themis_message_t* ctx);

#endif /* THEMIS_MESSAGE_H_ */
