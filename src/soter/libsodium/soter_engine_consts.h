/*
 * Copyright (c) 2017 Cossack Labs Limited
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

#ifndef SODIUM_SOTER_ENGINE_CONSTS_H_
#define SODIUM_SOTER_ENGINE_CONSTS_H_

#define SOTER_ASYM_PUBLIC  0x40
#define SOTER_ASYM_PRIVATE 0x80

#define SOTER_ASYM_KEY_TYPE SOTER_ASYM_PUBLIC|SOTER_ASYM_PRIVATE 

#define SOTER_ASYM_X25519 0x30
#define SOTER_ASYM_ED25519  0x40

#define SOTER_ASYM_KA_DEFAULT_ALG SOTER_ASYM_X25519
#define SOTER_ASYM_SIGN_DEFAULT_ALG SOTER_ASYM_ED25519

/*
 * @defgroup SOTER_SYM_ALGORYTHMS_IDS symmetric encription/decryption  algorithms ids
 * @brief supported symmetric encryption/decription algorithms definitions
 * @{
 */
#define SOTER_AEAD_SYM_XSALSA20_POLY1305 0x10000000

/** @} */

#endif /* SODIUM_SOTER_ENGINE_CONSTS_H_ */
