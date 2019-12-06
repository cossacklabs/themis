/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "test_utils.h"
#include <string.h>
#include "../common/sput.h"
#include <soter/soter.h>
#include <stdio.h>

static uint8_t parse_char(char c)
{
	if ((c >= '0') && (c <= '9'))
		return (uint8_t)(c - '0');

	if ((c >= 'A') && (c <= 'F'))
		return (uint8_t)(c - 'A' + 0xa);

	if ((c >= 'a') && (c <= 'f'))
		return (uint8_t)(c - 'a' + 0xa);

	return 0xff;
}

test_utils_status_t string_to_bytes(const char *str, uint8_t *bytes, size_t size)
{
	size_t str_size = strlen(str);
	size_t i;

	/* strings with hex data should at least be even in size */
	if (0x1 & str_size)
		return SOTER_INVALID_PARAMETER;

	/* check if we have enough memory for output */
	if (size < (str_size / 2))
		return SOTER_BUFFER_TOO_SMALL;

	for (i = 0; i < str_size; i += 2)
	{
		uint8_t upper = parse_char(str[i]);
		uint8_t lower = parse_char(str[i + 1]);

		if ((0xf0 & upper) || (0xf0 & lower))
			return SOTER_INVALID_PARAMETER;

		bytes[i / 2] = lower | (upper << 4);
	}

	return SOTER_SUCCESS;
}

size_t rand_int(size_t max_val)
{
	size_t res;

	do
	{
		if (soter_rand((uint8_t*)(&res), sizeof(res)) != SOTER_SUCCESS)
		{
			exit(1);
		}
		res %= max_val;
	} while (!res);

	return res;
}

void testsuite_start_testing(void)
{
	sput_start_testing();
}

void testsuite_enter_suite(const char *suite_name)
{
	sput_enter_suite(suite_name);
}

void testsuite_run_test_in_file(void (*test_func)(void), const char *test_func_name, const char *test_file_name)
{
	sput_run_test(test_func, test_func_name, test_file_name);
}

void testsuite_finish_testing(void)
{
	sput_finish_testing();
}

void testsuite_fail_if_on_line(bool condition, const char *name, unsigned long line)
{
	sput_fail_if(condition, name, line);
}

void testsuite_fail_unless_on_line(bool condition, const char *name, unsigned long line)
{
	sput_fail_unless(condition, name, line);
}

int testsuite_get_return_value(void)
{
	return sput_get_return_value();
}
