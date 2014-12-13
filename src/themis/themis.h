/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS_H
#define THEMIS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef int themis_status_t; 

themis_status_t themis_gen_key_pair(uint8_t* private_key,
				    size_t* private_key_length,
				    uint8_t* public_key,
				    size_t* public_key_length);

themis_status_t wrap_message(const uint8_t* message,
			     const uint8_t* private_key,
			     const size_t private_key_length,
			     const uint8_t* peer_public_key,
			     const size_t peer_public_key_length,
			     uint8_t* protected_message,
			     size_t* protected_message_length);

themis_status_t unwrap_message(const uint8_t* protected_message,
			       const size_t protected_message_length,
			       const uint8_t* private_key,
			       const size_t private_key_length,
			       const uint8_t* peer_public_key,
			       const size_t peer_public_key_length,
			       uint8_t* message,
			       size_t* message_length);

#endif /* THEMIS_H */



















