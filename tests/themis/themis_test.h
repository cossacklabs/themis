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

#ifndef THEMIS_TEST_H
#define THEMIS_TEST_H

#include <themis/themis.h>

#include <common/test_utils.h>

void run_secure_message_test(void);
void run_secure_session_test(void);
void run_secure_cell_test(void);
void run_secure_comparator_test(void);

#endif /* THEMIS_TEST_H */
