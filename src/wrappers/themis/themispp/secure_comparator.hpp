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

#ifndef THEMISPP_SECURE_COMPARATOR_HPP_
#define THEMISPP_SECURE_COMPARATOR_HPP_

#include <cstring>
#include <vector>

#include <themis/themis.h>

#include "exception.hpp"

namespace themispp
{

class secure_comparator_t
{
public:
    typedef std::vector<uint8_t> data_t;

    secure_comparator_t(const data_t& shared_secret)
        : comparator_(NULL)
    {
        if (shared_secret.empty()) {
            throw themispp::exception_t("Secure Comparator must have non-empty shared secret");
        }
        res_.reserve(512);
        comparator_ = secure_comparator_create();
        if (!comparator_) {
            throw themispp::exception_t("Secure Comparator construction failed");
        }
        themis_status_t status = secure_comparator_append_secret(comparator_,
                                                                 &shared_secret[0],
                                                                 shared_secret.size());
        if (THEMIS_SUCCESS != status) {
            throw themispp::exception_t("Secure Comparator failed to append secret", status);
        }
    }

    virtual ~secure_comparator_t()
    {
        if (comparator_) {
            secure_comparator_destroy(comparator_);
            comparator_ = NULL;
        }
    }

    const data_t& init()
    {
        themis_status_t status = THEMIS_FAIL;
        size_t data_length = 0;
        status = secure_comparator_begin_compare(comparator_, NULL, &data_length);
        if (THEMIS_BUFFER_TOO_SMALL != status) {
            throw themispp::exception_t("Secure Comparator failed to initialize comparison", status);
        }
        res_.resize(data_length);
        status = secure_comparator_begin_compare(comparator_, &res_[0], &data_length);
        if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != status) {
            throw themispp::exception_t("Secure Comparator failed to initialize comparison", status);
        }
        return res_;
    }

    const data_t& proceed(const std::vector<uint8_t>& data)
    {
        if (data.empty()) {
            throw themispp::exception_t(
                "Secure Comparator failed to proceed comparison: data must be non-empty");
        }
        themis_status_t status = THEMIS_FAIL;
        size_t res_data_length = 0;
        status = secure_comparator_proceed_compare(comparator_, &data[0], data.size(), NULL, &res_data_length);
        if (THEMIS_BUFFER_TOO_SMALL != status) {
            throw themispp::exception_t("Secure Comparator failed to proceed comparison", status);
        }
        res_.resize(res_data_length);
        status = secure_comparator_proceed_compare(comparator_, &data[0], data.size(), &res_[0], &res_data_length);
        if (THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER != status && THEMIS_SUCCESS != status) {
            throw themispp::exception_t("Secure Comparator failed to proceed comparison", status);
        }
        return res_;
    }

    bool get() const
    {
        return THEMIS_SCOMPARE_MATCH == secure_comparator_get_result(comparator_);
    }

#if __cplusplus >= 201103L
    secure_comparator_t(const secure_comparator_t&) = delete;
    secure_comparator_t& operator=(const secure_comparator_t&) = delete;

    secure_comparator_t(secure_comparator_t&& other) noexcept
    {
        comparator_ = other.comparator_;
        other.comparator_ = nullptr;
        res_ = std::move(other.res_);
    }

    secure_comparator_t& operator=(secure_comparator_t&& other) noexcept
    {
        if (this != &other) {
            if (comparator_) {
                secure_comparator_destroy(comparator_);
            }
            comparator_ = other.comparator_;
            other.comparator_ = nullptr;
            res_ = std::move(other.res_);
        }
        return *this;
    }
#else
private:
    secure_comparator_t(const secure_comparator_t&);
    secure_comparator_t& operator=(const secure_comparator_t&);
#endif

private:
    ::secure_comparator_t* comparator_;
    std::vector<uint8_t> res_;
};

} // namespace themispp

#endif
