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

#ifndef SOTER_API_H
#define SOTER_API_H

#if defined(__GNUC__) || defined(__clang__)
#define SOTER_API __attribute__((visibility("default")))
#elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#ifdef SOTER_EXPORT
#define SOTER_API __declspec(dllexport)
#else
#define SOTER_API __declspec(dllimport)
#endif
#else
#define SOTER_API
#endif

/*
 * Marks API that needs to be exported for technical reasons, but otherwise
 * is not intended for user consumption.
 */
#define SOTER_PRIVATE_API SOTER_API

#endif /* SOTER_API_H */
