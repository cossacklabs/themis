/*
 * Copyright (c) 2019 Cossack Labs Limited
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

#ifndef THEMISPP_RAND_TEST
#define THEMISPP_RAND_TEST

#include <vector>

#include <common/sput.h>

#include <themispp/secure_rand.hpp>

namespace themispp
{
namespace secure_rand_test
{

static bool not_all_zeros(const std::vector<uint8_t>& data)
{
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] != 0) {
            return true;
        }
    }
    return false;
}

static void secure_rand_test()
{
    themispp::secure_rand_t<32> random_32_bytes;

    std::vector<uint8_t> result = random_32_bytes.get();
    sput_fail_unless(result.size() == 32, "return 32 random bytes", __LINE__);
    sput_fail_unless(not_all_zeros(result), "they are actually random", __LINE__);

    std::vector<uint8_t> another = random_32_bytes.get();
    sput_fail_unless(another != result, "random bytes do not repeat", __LINE__);

    try {
        themispp::secure_rand_t<0> no_random_bytes;
        no_random_bytes.get();
        sput_fail_unless(false, "throws on zero size", __LINE__);
    } catch (themispp::exception_t& e) {
        sput_fail_unless(true, "throws on zero size", __LINE__);
    }
}

inline void run_secure_rand_test()
{
    sput_enter_suite("ThemisPP secure rand test:");
    sput_run_test(secure_rand_test, "secure_rand_test", __FILE__);
}

} // namespace secure_rand_test
} // namespace themispp

#endif /* THEMISPP_RAND_TEST */
