/**
 * @file
 *
 * (c) CossackLabs
 */

#include "soter/error.h"
#include "soter/soter.h"
#include <openssl/rand.h>

soter_status_t soter_rand(uint8_t* buffer, size_t length)
{
	if ((!buffer) || (!length))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (RAND_bytes(buffer, (int)length))
	{
		return SOTER_SUCCESS;
	}
	else
	{
		/* For some reason OpenSSL generator failed */
		return SOTER_FAIL;
	}
}
