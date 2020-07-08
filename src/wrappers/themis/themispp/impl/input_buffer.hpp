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

#ifndef THEMISPP_IMPL_INPUT_BUFFER_HPP_
#define THEMISPP_IMPL_INPUT_BUFFER_HPP_

#include <cstring>
#include <iterator>

#include <themis/themis.h>

#include "compat_begin.hpp"
namespace themispp
{
namespace impl
{

/**
 * Reference to a read-only byte buffer.
 *
 * @see input_bytes
 * @see input_string
 */
class input_buffer
{
public:
    explicit input_buffer() noexcept
        : m_data(0)
        , m_size(0)
    {
    }

    explicit input_buffer(const uint8_t* data, size_t size) noexcept
        : m_data(data)
        , m_size(size)
    {
    }

    const uint8_t* data() const noexcept
    {
        return m_data;
    }

    size_t size() const noexcept
    {
        return m_size;
    }

    bool empty() const noexcept
    {
        return !m_data || !m_size;
    }

    const uint8_t* begin() const noexcept
    {
        return m_data;
    }

    const uint8_t* end() const noexcept
    {
        // We assume the whole buffer is valid, and one-past-end pointers
        // are acceptable API in C++. They should not be dereferenced.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return m_data + m_size;
    }

private:
    const uint8_t* m_data;
    size_t m_size;
};

// Trivial constructors from self
inline input_buffer input_bytes(const input_buffer& other) noexcept
{
    return other;
}

#if __cplusplus >= 201103L
inline input_buffer input_bytes(input_buffer&& other) noexcept
{
    return other;
}
#endif

// C arrays of dynamic size
inline input_buffer input_bytes(const uint8_t* array, size_t size) noexcept
{
    return input_buffer(array, size);
}

// C arrays of static size
template <typename T, size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
inline input_buffer input_bytes(const T (&array)[N]) noexcept
{
    return input_buffer(array, N);
}

// Directly specified pair of contiguous iterators over bytes
template <typename Iterator>
inline input_buffer input_bytes(Iterator begin, Iterator end) noexcept
{
    // Until C++20 there is no way to check at compile-time whether iterators
    // are indeed contiguous, so we trust the application code with that.
    // The behavior is undefined (i.e., we'll crash) if memory between
    // begin and end is not addressable.
    if (begin == end) {
        return input_buffer();
    }
    return input_buffer(&*begin, end - begin);
}

// STL-compatible contiguous containers
template <typename Container>
inline input_buffer input_bytes(const Container& container) noexcept
{
    // data() is not guaranteed to return nullptr if it's empty
    if (container.empty()) {
        return input_buffer();
    }
    return input_buffer(container.data(), container.size());
}

// C++03 does not define data() and size() methods, and has exactly one
// container type which satisifes our requirements: vector.
#if __cplusplus < 201103L
template <typename Allocator>
inline input_buffer input_bytes(const std::vector<uint8_t, Allocator>& vector) noexcept
{
    // Cannot dereference front() if vector is empty
    if (vector.empty()) {
        return input_buffer();
    }
    return input_buffer(&vector.front(), vector.size());
}
#endif

// C arrays of dynamic size
inline input_buffer input_string(const uint8_t* array, size_t size) noexcept
{
    return input_buffer(array, size);
}

inline input_buffer input_string(const char* array, size_t size) noexcept
{
    // Here's our only reinterpret_cast. It is safe because we know that
    // "char" and "uint8_t" have the same memory alignment.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return input_buffer(reinterpret_cast<const uint8_t*>(array), size);
}

// C arrays of static size
template <typename T, size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
inline input_buffer input_string(const T (&array)[N]) noexcept
{
    return input_string(array, N);
}

// Null-terminated C strings
inline input_buffer input_string(const char* str) noexcept
{
    if (str) {
        return input_string(str, strlen(str));
    }
    return input_buffer();
}

// Sometimes you get "char*" and it throws off template deduction.
// Provide an explicit overload for this case.
inline input_buffer input_string(char* str) noexcept
{
    return input_string(static_cast<const char*>(str));
}

// Directly specified pair of contiguous iterators over chars
template <typename Iterator>
inline input_buffer input_string(Iterator begin, Iterator end) noexcept
{
    // Until C++20 there is no way to check at compile-time whether iterators
    // are indeed contiguous, so we trust the application code with that.
    // The behavior is undefined (i.e., we'll crash) if memory between
    // begin and end is not addressable.
    if (begin < end) {
        return input_string(&*begin, end - begin);
    }
    if (begin == end) {
        return input_buffer();
    }
    return input_string(&*end, begin - end);
}

// STL-compatible contiguous containers
template <typename Container>
inline input_buffer input_string(const Container& container) noexcept
{
    // data() is not guaranteed to return nullptr if it's empty
    if (container.empty()) {
        return input_buffer();
    }
    return input_string(container.data(), container.size());
}

// C++03 does not define data() and size() methods, and has exactly two
// container types which satisify our requirements: vector and basic_string.
#if __cplusplus < 201103L
template <typename T, typename Allocator>
inline input_buffer input_string(const std::vector<T, Allocator>& vector) noexcept
{
    // Cannot dereference front() if vector is empty
    if (vector.empty()) {
        return input_buffer();
    }
    return input_string(&vector.front(), vector.size());
}

template <typename T, typename Traits, typename Allocator>
inline input_buffer input_string(const std::basic_string<T, Traits, Allocator>& string) noexcept
{
    // data() is not guaranteed to return nullptr if it's empty
    if (string.empty()) {
        return input_buffer();
    }
    return input_string(string.data(), string.size());
}
#endif

} // namespace impl
} // namespace themispp
#include "compat_end.hpp"

#endif // THEMISPP_IMPL_INPUT_BUFFER_HPP_
