/**
 * @file
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"
#include <openssl/rand.h>

soter_status_t soter_rand(uint8_t** buffer, size_t length)
{
	uint8_t *bytes;

	if ((!buffer) || (!length))
		return HERMES_INVALID_PARAMETER;

	bytes = *buffer;

	if (!bytes)
	{
		bytes = malloc(length);
		if (!bytes)
			return HERMES_NO_MEMORY;
	}

	if (RAND_bytes(bytes, (int)length))
	{
		*buffer = bytes;
		return HERMES_SUCCESS;
	}
	else
	{
		/* If we allocated the buffer, free it */
		if (!(*buffer))
			free(bytes);

		/* For some reason OpenSSL generator failed */
		return HERMES_FAIL;
	}
}
