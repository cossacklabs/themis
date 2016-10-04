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

#include "soter_test.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#ifdef NIST_STS_EXE_PATH

#define _TO_STRING_(_X_) (#_X_)
#define TO_STRING(_X_) _TO_STRING_(_X_)

/* 1 MB */
#define TEST_BIT_STREAM_SAMPLE_LENGTH (8 * 1024 * 1024)

/* 8 samples of TEST_BIT_STREAM_SAMPLE_LENGTH */
#define TEST_BIT_STREAM_SAMPLE_COUNT 8

#define TEST_DATA_FILE_NAME "rng.bin"
#define REPORT_FILE_PATH "experiments/AlgorithmTesting/finalAnalysisReport.txt"

static bool generate_test_data(void)
{
	FILE *test_file = fopen(TEST_DATA_FILE_NAME, "wb");
	uint8_t rand_buf[1024];
	soter_status_t res;
	size_t bytes_written;
	size_t i;

	if (!test_file)
	{
		return false;
	}

	for (i = 0; i < ((TEST_BIT_STREAM_SAMPLE_LENGTH * TEST_BIT_STREAM_SAMPLE_COUNT) / 8); )
	{
		res = soter_rand(rand_buf, sizeof(rand_buf));
		if (SOTER_SUCCESS != res)
		{
			fclose(test_file);
			return false;
		}

		bytes_written = fwrite(rand_buf, 1, sizeof(rand_buf), test_file);
		if (bytes_written != sizeof(rand_buf))
		{
			fclose(test_file);
			return false;
		}

		i += bytes_written;
	}

	fclose(test_file);
	return true;
}

static bool run_nist_suite(void)
{
	FILE *nist_file = popen("./assess 1000000 > /dev/null", "w");
	char cmd[128];

	if (!nist_file)
	{
		return false;
	}

	sprintf(cmd, "0\n%s\n1\n0\n%d\n1\n", TEST_DATA_FILE_NAME, TEST_BIT_STREAM_SAMPLE_COUNT);
	fputs(cmd, nist_file);

	if (-1 == pclose(nist_file))
	{
		return false;
	}

	return true;
}

static bool get_result_from_report(void)
{
	FILE *report_file = fopen(REPORT_FILE_PATH, "r");
	char report_line[256];
	bool is_file_correct = false;
	bool failed_samples = false;

	if (!report_file)
	{
		return false;
	}

	while (fgets(report_line, sizeof(report_line), report_file))
	{
		if (!is_file_correct)
		{
			/* Sanity check: if we used correct input */
			if (strstr(report_line, TEST_DATA_FILE_NAME))
			{
				is_file_correct = true;
			}
		}

		/* Failed tests are marked with '*' in the report */
		if (strchr(report_line, '*'))
		{
			failed_samples = true;
		}
	}

	fclose(report_file);

	return (is_file_correct && !failed_samples);
}

/* NIST test suite should be launched from its root directory */
static void test_rand_with_nist(void)
{
	char curr_work_dir[PATH_MAX];

	/* Store current work dir */
	if (NULL == getcwd(curr_work_dir, sizeof(curr_work_dir)))
	{
		testsuite_fail_if(true, "getcwd failed");
		return;
	}

	/* Change to NIST directory */
	if (0 != chdir(TO_STRING(NIST_STS_EXE_PATH)))
	{
		testsuite_fail_if(true, "chdir failed");
		return;
	}

	if (!generate_test_data())
	{
		testsuite_fail_if(true, "generate_test_data failed");
		return;
	}

	if (!run_nist_suite())
	{
		testsuite_fail_if(true, "run_nist_suite failed");
		return;
	}

	testsuite_fail_unless(get_result_from_report(), "NIST tests");

	/* Change back to original directory */
	if (0 != chdir(curr_work_dir))
	{
		testsuite_fail_if(true, "chdir failed");
		return;
	}
}

#else

static void test_rand_with_nist(void)
{
	testsuite_fail_if(true, "NIST tests");
}

#endif

static void test_api(void)
{
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_rand(NULL, 32), "soter_rand: invalid out buffer");
	testsuite_fail_unless(SOTER_INVALID_PARAMETER == soter_rand((uint8_t *)0x1, 0), "soter_rand: invalid buffer length");
}

void run_soter_rand_tests(void)
{
	testsuite_enter_suite("soter rand: api");
	testsuite_run_test(test_api);

//	testsuite_enter_suite("soter rand: NIST STS (make take some time...)");
//	testsuite_run_test(test_rand_with_nist);
}
