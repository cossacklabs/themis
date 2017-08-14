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

#ifndef OPENSSL_SOTER_ENGINE_CONSTS_H_
#define OPENSSL_SOTER_ENGINE_CONSTS_H_

#define SOTER_HASH_SHA256 1
#define SOTER_HASH_SHA512 2

#define SOTER_HASH_DEFAULT_ALG SOTER_HASH_SHA256


#define SOTER_ASYM_PUBLIC  0x00000055 //"U\x00\x00\x00"
#define SOTER_ASYM_PRIVATE 0x00000052 //"R\x00\x00\x00"

#define SOTER_ASYM_KEY_TYPE SOTER_ASYM_PUBLIC|SOTER_ASYM_PRIVATE 

#define SOTER_ASYM_RSA 0x00415200 //"\x00RA\x00"
#define SOTER_ASYM_EC  0x00434500 //"\x00EC\x00"

#define SOTER_ASYM_ALG_MASK 0x00ffff00

#define SOTER_ASYM_RSA_LENGTH_1024 0x31000000 //"\x00\x00\x001"
#define SOTER_ASYM_RSA_LENGTH_2048 0x32000000 //"\x00\x00\x002"
#define SOTER_ASYM_RSA_LENGTH_4096 0x34000000 //"\x00\x00\x004"
#define SOTER_ASYM_RSA_LENGTH_8192 0x38000000 //"\x00\x00\x008"

#define SOTER_ASYM_EC_LENGTH_256 0x32000000 //"\x00\x00\x002"
#define SOTER_ASYM_EC_LENGTH_384 0x33000000 //"\x00\x00\x003"
#define SOTER_ASYM_EC_LENGTH_521 0x35000000 //"\x00\x00\x005"

//#define SOTER_ASYM_KEY_LENGTH 0x7

#define SOTER_ASYM_CIPHER_DEFAULT_ALG (SOTER_ASYM_RSA|SOTER_ASYM_RSA_LENGTH_2048)
#define SOTER_ASYM_KA_DEFAULT_ALG (SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_256)
#define SOTER_ASYM_SIGN_DEFAULT_ALG (SOTER_ASYM_EC|SOTER_ASYM_EC_LENGTH_256)

/*
 * @defgroup SOTER_SYM_ALGORYTHMS_IDS symmetric encription/decryption  algorithms ids
 * @brief supported symmetric encryption/decription algorithms definitions
 * @{
 */

/** AES in ECB mode with pkcs7 padding */
#define SOTER_SYM_AES_ECB_PKCS7     0x10010000
/** AES in CTR mode */
#define SOTER_SYM_AES_CTR           0x20000000
/** AES in XTS mode */
#define SOTER_SYM_AES_XTS           0x30000000
/** AES in GCM mode (with authenticated encryption) */
#define SOTER_SYM_AES_GCM           0x40010000

/** @} */

/** 
 * @defgroup SOTER_KDF_ALGS kdf algorithms
 * @brief supported kdf algorithms
 * @{
*/
/** do not use kdf */
#define SOTER_SYM_NOKDF             0x00000000
/** pbkdf2 algorythm */
#define SOTER_SYM_PBKDF2            0x01000000
/** @} */

/** 
 * @defgroup SOTER_KEY_LENGTH supported lengths of keys
 * @brief supported lengths of keys
 * @{
*/
/** 256 bits */
#define SOTER_SYM_256_KEY_LENGTH    0x00000100
/** 192 bits */
#define SOTER_SYM_192_KEY_LENGTH    0x000000c0
/** 512 bits */
#define SOTER_SYM_128_KEY_LENGTH    0x00000080

/** @} */

/** 
 * @defgroup SOTER_SYM_MASK masks definition for symmetryc algorithm id
 * @brief masks definition for symmetryc algorithm id
 * @{
 */
/** key length mask */
#define SOTER_SYM_KEY_LENGTH_MASK   0x00000fff
/** padding algorithm */
#define SOTER_SYM_PADDING_MASK      0x000f0000
/** encryption algorithm */
#define SOTER_SYM_ALG_MASK          0xf0000000
/** kdf algorithm */
#define SOTER_SYM_KDF_MASK          0x0f000000

/** @} */
/** @} */

#define SOTER_SYM_DEFAULT_ALG SOTER_SYM_AES_CTR|SOTER_SYM_256_KEY_LENGTH|SOTER_SYM_PBKDF2
#define SOTER_SYM_DEFAULT_ALG_KEY_LENGTH SOTER_SYM_256_KEY_LENGTH
#define SOTER_SYM_DEFAULT_ALG_IV_SIZE 12
#define SOTER_SYM_DEFAULT_ALG_MAX_BLOCK_SIZE 16

#define SOTER_SYM_AEAD_DEFAULT_ALG SOTER_SYM_AES_GCM|SOTER_SYM_256_KEY_LENGTH|SOTER_SYM_PBKDF2
#define SOTER_SYM_AEAD_DEFAULT_ALG_KEY_LENGTH SOTER_SYM_256_KEY_LENGTH
#define SOTER_SYM_AEAD_DEFAULT_ALG_IV_SIZE 12
#define SOTER_SYM_AEAD_DEFAULT_ALG_AUTH_TAG_SIZE 16
#define SOTER_SYM_AEAD_DEFAULT_ALG_MAX_BLOCK_SIZE 16

/**
 * @defgroup SOTER_SYM_ROUTINES symmetric encryption/decryption routines
 * @brief symmetric encryption/decryption routines
 * @{
 */
  

#endif /* OPENSSL_SOTER_ENGINE_CONSTS_H_ */
