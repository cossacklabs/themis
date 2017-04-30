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

#define SOTER_HASH_BLAKE2 1
#define SOTER_HASH_DEFAULT_ALG SOTER_HASH_BLAKE2

#define SOTER_ASYM_PUBLIC  0x40
#define SOTER_ASYM_PRIVATE 0x80

#define SOTER_ASYM_KEY_TYPE SOTER_ASYM_PUBLIC|SOTER_ASYM_PRIVATE 

#define SOTER_ASYM_X25519 0x30
#define SOTER_ASYM_ED25519  0x40

#define SOTER_ASYM_KA_DEFAULT_ALG SOTER_ASYM_X25519
#define SOTER_ASYM_SIGN_DEFAULT_ALG SOTER_ASYM_ED25519
#define SOTER_ASYM_CIPHER_DEFAULT_ALG 0

/*
 * @defgroup SOTER_SYM_ALGORYTHMS_IDS symmetric encription/decryption  algorithms ids
 * @brief supported symmetric encryption/decription algorithms definitions
 * @{
 */
#define SOTER_SYM_AEAD_SYM_CHACHA20_POLY1305 0x10000000
#define SOTER_SYM_AEAD_DEFAULT_ALG SOTER_SYM_AEAD_SYM_CHACHA20_POLY1305
#define SOTER_SYM_AEAD_DEFAULT_ALG_KEY_LENGTH (crypto_aead_chacha20poly1305_KEYBYTES*8)
#define SOTER_SYM_AEAD_DEFAULT_ALG_IV_SIZE crypto_secretbox_NONCEBYTES
#define SOTER_SYM_AEAD_DEFAULT_ALG_AUTH_TAG_SIZE crypto_aead_chacha20poly1305_ABYTES
#define SOTER_SYM_AEAD_DEFAULT_ALG_MAX_BLOCK_SIZE 16

#define SOTER_SYM_DEFAULT_ALG 0
#define SOTER_SYM_DEFAULT_ALG_KEY_LENGTH 0
#define SOTER_SYM_DEFAULT_ALG_IV_SIZE 0
#define SOTER_SYM_DEFAULT_ALG_MAX_BLOCK_SIZE 0
/** @} */

#endif /* SODIUM_SOTER_ENGINE_CONSTS_H_ */
