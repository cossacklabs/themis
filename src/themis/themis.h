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

/**
 * @file themis.h
 * @brief main header
 */
#ifndef THEMIS_H
#define THEMIS_H

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup THEMIS Themis
 * @brief Data security library for network communication and data storage
 * @{
 */

#define THEMIS_VERSION_TEXT "themis 0.9: "

#include <themis/error.h>
#include <themis/secure_message.h>
#include <themis/secure_cell.h>
#include <themis/secure_session.h>

#ifdef SECURE_COMPARATOR_ENABLED
#include <themis/secure_comparator.h>
#endif

const char* themis_version();

/** @} */
#endif /* THEMIS_H */



















