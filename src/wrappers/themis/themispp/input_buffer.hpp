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

#ifndef THEMISPP_INPUT_BUFFER_HPP_
#define THEMISPP_INPUT_BUFFER_HPP_

#include "impl/input_buffer.hpp"

#include "impl/compat_begin.hpp"
namespace themispp
{

/**
 * View container as input buffer.
 *
 * @param [in]  container   STL-compatible container of bytes
 *
 * `container` must be an STL-compatible contiguous container of bytes. That is,
 * there should be `std::begin()` and `std::end()` implementations for it which
 * return contiguous iterators over `uint8_t`.
 *
 * The following standard containers satisfy the requirements:
 *
 * - `std::array<uint8_t>`
 * - `std::vector<uint8_t>`
 * - fixed-size C arrays of `uint8_t`
 *
 * @returns Input buffer for use with ThemisPP functions.
 */
template <typename Container>
inline impl::input_buffer input_buffer(const Container& container) noexcept
{
    return impl::input_bytes(container);
}

/**
 * View a pair of iterators as input buffer.
 *
 * @param [in]  begin   STL-compatible iterator over bytes
 * @param [in]  end     STL-compatible iterator over bytes
 *
 * `begin` and `end` must be STL-compatible contiguous iterators over `uint8_t`.
 *
 * The following standard containers produce appropriate iterators:
 *
 * - `std::array<uint8_t>`
 * - `std::vector<uint8_t>`
 *
 * You can also pointers (`const uint8_t*`).
 *
 * @returns Input buffer for use with ThemisPP functions.
 */
template <typename Iterator>
inline impl::input_buffer input_buffer(Iterator begin, Iterator end) noexcept
{
    return impl::input_bytes(begin, end);
}

} // namespace themispp
#include "impl/compat_end.hpp"

#endif // THEMISPP_INPUT_BUFFER_HPP_
