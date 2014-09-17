/**
 * @file
 * @brief tests for Soter
 *
 * (c) CossackLabs
 */

#include <soter.h>
#include <stdio.h>

static void print_bytes(const uint8_t *bytes, size_t length)
{
	size_t i;

	for (i = 0; i < length; i++)
		printf("%02x", bytes[i]);
	puts("");
}

/* For now, just generate some random bits and print them */
static void test_rand(void)
{
	uint8_t *bytes = NULL;
	size_t length = 4096;

	if (SOTER_SUCCESS == soter_rand(&bytes, length))
	{
		print_bytes(bytes, length);
		free(bytes);
	}
}

int main()
{
	test_rand();

    return 0;
}
