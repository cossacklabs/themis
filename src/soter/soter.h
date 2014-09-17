/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_H
#define SOTER_H

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Function return codes
 *
 * TODO: Should define detailed error codes. What codes will we use for errors? It would be good to make them consistent with errno.h where applicable.
 */
enum soter_status_type
{
	SOTER_SUCCESS = 0,
	SOTER_FAIL = -1,
	SOTER_INVALID_PARAMETER = -2,
	SOTER_NO_MEMORY = -3
};

typedef enum soter_status_type soter_status_t;

/**
 * @brief Generates random bits
 *
 * @param [in,out] buffer pointer to pointer to a buffer for random bits
 * @param [in] length length of the buffer
 *
 * This function generates random bits and puts them in memory pointed by buffer pointer. If buffer points to NULL, new memory of size length will be allocated on the heap and the pointer to this memory will be returned using buffer pointer. Otherwise, random bits will be stored in memory pointed by pointer in buffer. It is the responsibility of the caller to free the memory, if it was allocated by this function.
 */
soter_status_t soter_rand(uint8_t** buffer, size_t length);

#endif /* SOTER_H */
