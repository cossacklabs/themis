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
 * @file soter.h
 *
 * @brief main interface of soter
 * @see soter in wiki
 */
#ifndef SOTER_H
#define SOTER_H


/** 
 * @defgroup SOTER Soter 
 * @brief Soter is a cross-platform multipurpose cryptographic library. It provides a set of highly secure cryptographic primitives through a well-defined, consistent and simple interface.
 * @{
 */

#define SOTER_VERSION_TEXT "soter 0.9: "

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <soter/error.h>
#include <soter/soter_rand.h>
#include <soter/soter_hash.h>
#include <soter/soter_hmac.h>
#include <soter/soter_sym.h>
#include <soter/soter_rsa_key_pair_gen.h>
#include <soter/soter_asym_cipher.h>
#include <soter/soter_asym_ka.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_kdf.h>

/** 
 *  @brief get version string of soter
 * 
 *  @return version string
 *
 */
const char* soter_version();
/**@}*/
#endif /* SOTER_H */
