/**
 * @file
 * @brief utility functions for tests
 *
 * (c) CossackLabs
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "common/error.h"
#include <stdint.h>
#include <stdlib.h>

typedef int test_utils_status_t;

test_utils_status_t string_to_bytes(const char *str, uint8_t *bytes, size_t size);

#endif /* TEST_UTILS_H */
