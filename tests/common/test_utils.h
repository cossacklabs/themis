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

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <themis/themis_error.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef int test_utils_status_t;

test_utils_status_t string_to_bytes(const char *str, uint8_t *bytes, size_t size);
size_t rand_int(size_t max_val);

/* These are wrappers for sput test framework. Since sput uses a static state variable, when called from different files, it resets test number count (since for each file different test context is used). */
void testsuite_start_testing(void);
void testsuite_enter_suite(const char *suite_name);
void testsuite_run_test_in_file(void (*test_func)(void), const char *test_func_name, const char *test_file_name);
void testsuite_finish_testing(void);
void testsuite_fail_if_on_line(bool condition, const char *name, unsigned long line);
void testsuite_fail_unless_on_line(bool condition, const char *name, unsigned long line);
int testsuite_get_return_value(void);

#define testsuite_fail_if(_COND_, _NAME_) testsuite_fail_if_on_line(_COND_, _NAME_, __LINE__)
#define testsuite_fail_unless(_COND_, _NAME_) testsuite_fail_unless_on_line(_COND_, _NAME_, __LINE__)
#define testsuite_run_test(_FUNC_) testsuite_run_test_in_file(_FUNC_, #_FUNC_, __FILE__)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_H */
