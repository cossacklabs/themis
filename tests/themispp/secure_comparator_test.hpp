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

#ifndef THEMISPP_SECURE_COMPARATOR_TEST_HPP_
#define THEMISPP_SECURE_COMPARATOR_TEST_HPP_

#include <string.h>

#include <common/sput.h>

#include <themispp/secure_comparator.hpp>

#include "utils.hpp"

namespace themispp
{
namespace secure_session_test
{

static void secure_comparator_test()
{
    themispp::secure_comparator_t a(as_bytes("shared_secret"));
    themispp::secure_comparator_t b(as_bytes("shared_secret"));

    std::vector<uint8_t> buf;

    buf = a.init();
    buf = b.proceed(buf);
    buf = a.proceed(buf);
    buf = b.proceed(buf);
    buf = a.proceed(buf);

    sput_fail_unless(a.get(), "a ready", __LINE__);
    sput_fail_unless(b.get(), "b ready", __LINE__);

    themispp::secure_comparator_t c(as_bytes("shared_secret_c"));
    themispp::secure_comparator_t d(as_bytes("shared_secret_d"));

    buf = c.init();
    buf = d.proceed(buf);
    buf = c.proceed(buf);
    buf = d.proceed(buf);
    buf = c.proceed(buf);

    sput_fail_unless(!(c.get()), "c ready", __LINE__);
    sput_fail_unless(!(d.get()), "d ready", __LINE__);
}

inline int run_secure_comparator_test()
{
    sput_enter_suite("ThemisPP secure comparator test");
    sput_run_test(secure_comparator_test, "secure_comparator_test", __FILE__);
    return 0;
}

} // namespace secure_session_test
} // namespace themispp

#endif
