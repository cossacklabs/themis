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

#include <common/sput.h>

#include "input_buffer_test.hpp"
#include "secret_bytes_test.hpp"
#include "secure_cell_test.hpp"
#include "secure_comparator_test.hpp"
#include "secure_message_test.hpp"
#include "secure_rand_test.hpp"
#include "secure_session_test.hpp"

int main()
{
    try {
        sput_start_testing();
        themispp::secure_cell_test::run_secure_cell_test();
        themispp::secure_message_test::run_secure_message_test();
        themispp::secure_session_test::run_secure_session_test();
        themispp::secure_session_test::run_secure_comparator_test();
        themispp::secure_rand_test::run_secure_rand_test();
        themispp::input_buffer_test::run_input_buffer_test();
        themispp::secret_bytes_test::run_secret_bytes_test();
        sput_finish_testing();
        return sput_get_return_value();
    } catch (const std::exception& e) {
        std::cerr << "unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
