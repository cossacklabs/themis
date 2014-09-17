/**
 * @file
 *
 * (c) CossackLabs
 */

#include <soter.h>
#include <openssl/rand.h>

soter_status_t soter_rand(uint8_t** buffer, size_t length)
{
	uint8_t *bytes;

	if ((!buffer) || (!length))
		return SOTER_INVALID_PARAMETER;

	bytes = *buffer;

	if (!bytes)
	{
		bytes = malloc(length);
		if (!bytes)
			return SOTER_NO_MEMORY;
	}

	if (RAND_bytes(bytes, (int)length))
	{
		*buffer = bytes;
		return SOTER_SUCCESS;
	}
	else
	{
		/* If we allocated the buffer, free it */
		if (!(*buffer))
			free(bytes);

		/* For some reason OpenSSL generator failed */
		return SOTER_FAIL;
	}
}
