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

#ifndef THEMISPP_IMPL_SECRET_BYTES_HPP_
#define THEMISPP_IMPL_SECRET_BYTES_HPP_

#include <vector>

#include <soter/soter_wipe.h>

#include "input_buffer.hpp"

#include "compat_begin.hpp"
namespace themispp
{
namespace impl
{

/**
 * Read-only storage for sensitive data.
 *
 * Wiped from memory on destruction.
 */
template <class Allocator = std::allocator<uint8_t> >
class secret_bytes
{
public:
    template <typename Container>
    explicit secret_bytes(const Container& container)
    {
        input_buffer buffer = input_bytes(container);
        m_data.resize(buffer.size());
        m_data.assign(buffer.begin(), buffer.end());
    }

#if __cplusplus >= 201103L
    // if the user can give us their vector then we'll take it and avoid copying the data
    explicit secret_bytes(std::vector<uint8_t, Allocator>&& vector)
        : m_data(std::move(vector))
    {
    }

    explicit secret_bytes(const secret_bytes& other) = default;
    explicit secret_bytes(secret_bytes&& other) noexcept = default;
    secret_bytes& operator=(const secret_bytes& other) = default;
    secret_bytes& operator=(secret_bytes&& other) noexcept = default;
#else
    explicit secret_bytes(const secret_bytes& other)
        : m_data(other.m_data)
    {
    }

    secret_bytes& operator=(const secret_bytes& other)
    {
        m_data = other.m_data;
        return *this;
    }
#endif

    ~secret_bytes()
    {
        if (!m_data.empty()) {
            // This makes applications depend on both libthemis and libsoter.
            // Is there a way to reexport soter_wipe() from libthemis?
            // Maybe we should consider adding a convenience themis_wipe()?
#if __cplusplus >= 201103L
            soter_wipe(m_data.data(), m_data.size());
#else
            soter_wipe(&m_data.front(), m_data.size());
#endif
        }
    }

    const uint8_t* data() const noexcept
    {
        if (m_data.empty()) {
            return 0;
        }
#if __cplusplus >= 201103L
        return m_data.data();
#else
        return &m_data.front();
#endif
    }

    size_t size() const noexcept
    {
        return m_data.size();
    }

    bool empty() const noexcept
    {
        return m_data.empty();
    }

private:
    std::vector<uint8_t, Allocator> m_data;
};

} // namespace impl
} // namespace themispp
#include "compat_end.hpp"

#endif // THEMISPP_IMPL_SECRET_BYTES_HPP_
