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

#ifndef THEMISPP_TEST_UTILS_HPP
#define THEMISPP_TEST_UTILS_HPP

#include <stdint.h>

#include <string>
#include <vector>

namespace themispp
{

inline std::vector<uint8_t> as_bytes(const std::string& s)
{
    return std::vector<uint8_t>(s.begin(), s.end());
}

// TODO: replace with initializer lists when C++03 support is dropped
template <size_t N>
inline std::vector<uint8_t> as_bytes(const uint8_t (&a)[N]) // NOLINT
{
    return std::vector<uint8_t>(a, a + N);
}

} // namespace themispp

#endif // THEMISPP_TEST_UTILS_HPP
