/*
 * Copyright (c) 2020 Cossack Labs Limited
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

#ifndef THEMISPP_SECRET_BYTES_TEST
#define THEMISPP_SECRET_BYTES_TEST

#include <string.h>
#include <vector>

#include <common/sput.h>

#include <themispp/impl/secret_bytes.hpp>

#include "utils.hpp"

namespace themispp
{
namespace secret_bytes_test
{

// Unfortunately, we have to spell it out. C++03 has no "auto".
typedef impl::secret_bytes<std::allocator<uint8_t> > secret_bytes;

static inline void test_secret_bytes()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    uint8_t array[] = {1, 2, 3};
    secret_bytes copy_array(array);
    sput_fail_unless(copy_array.size() == 3, "copy C array", __LINE__);
    sput_fail_unless(!memcmp(&array[0], copy_array.data(), copy_array.size()),
                     "copy C array: data",
                     __LINE__);

    std::vector<uint8_t> vector;
    vector.push_back(4);
    vector.push_back(5);
    vector.push_back(6);
    vector.push_back(7);
    secret_bytes copy_vector(vector);
    sput_fail_unless(copy_vector.size() == 4, "copy std::vector", __LINE__);
    sput_fail_unless(!memcmp(&vector[0], copy_vector.data(), copy_vector.size()),
                     "copy std::vector: data",
                     __LINE__);

#if __cplusplus >= 201103L
    std::vector<uint8_t> other_vector;
    other_vector.push_back(8);
    other_vector.push_back(9);
    secret_bytes move_vector(std::move(other_vector));
    sput_fail_unless(move_vector.size() == 2, "move std::vector", __LINE__);
    // GCC says "potential null pointer dereference" but that's false positive:
    // "move_vector.data()" is never null because "move_vector.size() == 2".
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    sput_fail_unless(move_vector.data()[0] == 8 && move_vector.data()[1] == 9,
                     "move std::vector: data",
                     __LINE__);
#pragma GCC diagnostic pop
    // NOLINTNEXTLINE(bugprone-use-after-move)
    sput_fail_unless(other_vector.empty(), "move std::vector: moved", __LINE__);
#endif
}

inline void run_secret_bytes_test()
{
    sput_enter_suite("themispp::impl::secret_bytes");
    sput_run_test(test_secret_bytes, "secret bytes", __FILE__);
    // I wish it was possible to actually test wiping without triggering
    // undefined behavior but that's not realistic. So... have faith.
}

} // namespace secret_bytes_test
} // namespace themispp

#endif // THEMISPP_SECRET_BYTES_TEST
