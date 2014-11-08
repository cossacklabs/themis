/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_RAND_H
#define SOTER_RAND_H

#include <soter/soter.h>

/**
 * @brief Generates random bits
 *
 * @param [out] buffer pointer to a buffer for random bits
 * @param [in] length length of the buffer
 * @return success code
 *
 * This function generates random bits and puts them in memory pointed by buffer.
 */
soter_status_t soter_rand(uint8_t* buffer, size_t length);

#endif /* SOTER_RAND_H */
