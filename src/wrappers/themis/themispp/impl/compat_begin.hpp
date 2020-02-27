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

#ifndef THEMISPP_IMPL_COMPAT_BEGIN_HPP_
#define THEMISPP_IMPL_COMPAT_BEGIN_HPP_

#ifdef THEMISPP_IMPL_COMPAT_END_HPP_
#undef THEMISPP_IMPL_COMPAT_END_HPP_
#endif

#if __cplusplus < 201103L
#define noexcept
#endif

#endif // THEMISPP_IMPL_COMPAT_BEGIN_HPP_
