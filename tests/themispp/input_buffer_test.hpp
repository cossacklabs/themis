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

#ifndef THEMISPP_INPUT_BUFFER_TEST
#define THEMISPP_INPUT_BUFFER_TEST

#include <stdint.h>

#if __cplusplus >= 201103L
#include <array>
#endif
#include <vector>

#include <common/sput.h>

#include <themispp/input_buffer.hpp>

#include "utils.hpp"

struct non_stl_container {
    std::vector<uint8_t> inner;
};

// How to teach Themis to work with your non-STL container:
namespace themispp
{
namespace impl
{

template <>
inline input_buffer input_bytes(const non_stl_container& container)
#if __cplusplus >= 201103L
    noexcept
#endif
{
    return input_bytes(container.inner);
}

} // namespace impl
} // namespace themispp

namespace themispp
{
namespace input_buffer_test
{

static inline void input_buffer_bytes()
{
#if __cplusplus >= 201103L
    {
        std::array<uint8_t, 0> array = {};
        themispp::impl::input_buffer buf = themispp::impl::input_bytes(array);
        sput_fail_unless(!buf.data() && buf.empty(), "std::array (empty)", __LINE__);
    }
    {
        std::array<uint8_t, 3> array = {0, 0, 0};
        themispp::impl::input_buffer buf = themispp::impl::input_bytes(array);
        sput_fail_unless(buf.data() == &array.at(0) && buf.size() == 3, "std::array (full)", __LINE__);
    }
#endif
    {
        std::vector<uint8_t> vector;
        themispp::impl::input_buffer buf = themispp::impl::input_bytes(vector);
        sput_fail_unless(!buf.data() && buf.empty(), "std::vector (empty)", __LINE__);
    }
    {
        std::vector<uint8_t> vector(10, 0);
        themispp::impl::input_buffer buf = themispp::impl::input_bytes(vector);
        sput_fail_unless(buf.data() == &vector[0] && buf.size() == 10, "std::vector (full)", __LINE__);
    }
    {
        std::vector<uint8_t> vector(5, 0);
        themispp::impl::input_buffer buf1 = themispp::impl::input_bytes(vector.begin(), vector.end());
        sput_fail_unless(buf1.data() == &vector[0] && buf1.size() == 5, "std::vector (iterators)", __LINE__);
    }
    {
        uint8_t array[] = {1, 2, 3}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
        size_t array_size = sizeof(array);
        const uint8_t* array_begin = &array[0];
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const uint8_t* array_end = &array[0] + array_size;
        themispp::impl::input_buffer buf1 = themispp::impl::input_bytes(array);
        sput_fail_unless(buf1.data() == array_begin && buf1.size() == 3, "C array (entire)", __LINE__);
        themispp::impl::input_buffer buf2 = themispp::impl::input_bytes(array_begin, array_end);
        sput_fail_unless(buf2.data() == array_begin && buf2.size() == 3,
                         "C array (limit pointers)",
                         __LINE__);
        themispp::impl::input_buffer buf3 = themispp::impl::input_bytes(array_begin, array_size);
        sput_fail_unless(buf3.data() == array_begin && buf3.size() == 3,
                         "C array (pointer+length)",
                         __LINE__);
    }
}

static inline const uint8_t* as_bytes(const char* p)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const uint8_t*>(p);
}

static inline void input_buffer_strings()
{
    {
        // plain "0", "NULL", "nullptr" does not even compile
        const char* null = 0;
        themispp::impl::input_buffer buf = themispp::impl::input_string(null);
        sput_fail_unless(!buf.data() && buf.empty(), "NULL", __LINE__);
    }
    {
        const char* string = "C string";
        themispp::impl::input_buffer buf = themispp::impl::input_string(string);
        sput_fail_unless(buf.data() == as_bytes(string) && buf.size() == 8, "C string", __LINE__);
    }
    {
        std::string string;
        themispp::impl::input_buffer buf = themispp::impl::input_string(string);
        sput_fail_unless(!buf.data() && buf.empty(), "std::string (empty)", __LINE__);
    }
    {
        std::string string("string");
        themispp::impl::input_buffer buf = themispp::impl::input_string(string);
        sput_fail_unless(buf.data() == as_bytes(string.c_str()) && buf.size() == 6,
                         "std::string (full)",
                         __LINE__);
    }
#if __cplusplus >= 201703L
    {
        std::string string("big string");
        std::string_view view = string;
        view.remove_prefix(4);
        view.remove_suffix(2);
        themispp::impl::input_buffer buf = themispp::impl::input_string(view);
        sput_fail_unless(buf.data() == as_bytes(string.c_str() + 4) && buf.size() == 4,
                         "std::string_view",
                         __LINE__);
    }
#endif
    {
        std::vector<uint8_t> vector(7, 0);
        themispp::impl::input_buffer buf = themispp::impl::input_string(vector);
        sput_fail_unless(buf.data() == &vector[0] && buf.size() == 7, "bytes are fine too", __LINE__);
    }
}

static inline void input_buffer_user_api_containers()
{
#if __cplusplus >= 201103L
    std::array<uint8_t, 3> array = {0, 0, 0};
    themispp::input_buffer(array);
    sput_fail_unless(true, "std::array", __LINE__);
#endif

    std::vector<uint8_t> vector(3, 0);
    themispp::input_buffer(vector);
    sput_fail_unless(true, "std::vector", __LINE__);

    uint8_t c_array[] = {0, 0, 0}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    themispp::input_buffer(c_array);
    sput_fail_unless(true, "C arrays", __LINE__);
}

static inline void input_buffer_user_api_iterators()
{
#if __cplusplus >= 201103L
    std::array<uint8_t, 3> array = {0, 0, 0};
    themispp::input_buffer(array.begin(), array.end());
    sput_fail_unless(true, "std::array", __LINE__);
#endif

    std::vector<uint8_t> vector(3, 0);
    themispp::input_buffer(vector.begin(), vector.end());
    sput_fail_unless(true, "std::vector", __LINE__);

    uint8_t c_array[] = {0, 0, 0}; // NOLINT(cppcoreguidelines-avoid-c-arrays)
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    themispp::input_buffer(&c_array[0], &c_array[0] + 3);
    sput_fail_unless(true, "C arrays", __LINE__);
}

static inline void input_buffer_user_api_specialize()
{
    non_stl_container container = {std::vector<uint8_t>(8, 0)};
    themispp::impl::input_buffer buf = themispp::impl::input_bytes(container);
    sput_fail_unless(buf.data() == &container.inner[0] && buf.size() == 8,
                     "custom specialization",
                     __LINE__);
}

inline void run_input_buffer_test()
{
    sput_enter_suite("themispp::input_buffer");
    sput_run_test(input_buffer_bytes, "byte buffers", __FILE__);
    sput_run_test(input_buffer_strings, "string buffers", __FILE__);
    sput_run_test(input_buffer_user_api_containers, "user API (containers)", __FILE__);
    sput_run_test(input_buffer_user_api_iterators, "user API (iterators)", __FILE__);
    sput_run_test(input_buffer_user_api_specialize, "user API (specialization)", __FILE__);
}

} // namespace input_buffer_test
} // namespace themispp

#endif // THEMISPP_INPUT_BUFFER_TEST
