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

#ifndef SOTER_RSA_KEY_H
#define SOTER_RSA_KEY_H

#include <soter/soter_container.h>
#include <soter/soter.h>

#include <arpa/inet.h>

#define RSA_PRIV_KEY_PREF "RRA"
#define RSA_PUB_KEY_PREF "URA"

#define RSA_1024 "1"
#define RSA_2048 "2"
#define RSA_4096 "4"
#define RSA_8192 "8"

#define RSA_KEY_LENGTH_1024 1
#define RSA_KEY_LENGTH_2048 2
#define RSA_KEY_LENGTH_4096 3
#define RSA_KEY_LENGTH_8192 4

#define RSA_KEY_SUF(_KEY_SIZE_) RSA_##_KEY_SIZE_

#define RSA_PRIV_KEY_TAG(_KEY_SIZE_) (RSA_PRIV_KEY_PREF RSA_KEY_SUF(_KEY_SIZE_))
#define RSA_PUB_KEY_TAG(_KEY_SIZE_) (RSA_PUB_KEY_PREF RSA_KEY_SUF(_KEY_SIZE_))

#define RSA_BYTE_SIZE(_KEY_SIZE_) (_KEY_SIZE_ / 8)

#define DECLARE_RSA_PUBLIC_KEY(_KEY_SIZE_) \
	struct soter_rsa_pub_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t mod[RSA_BYTE_SIZE(_KEY_SIZE_)]; \
		uint32_t pub_exp; /* Network byte order */ \
	}; \
	\
	typedef struct soter_rsa_pub_key_##_KEY_SIZE_##_type soter_rsa_pub_key_##_KEY_SIZE_##_t

/* Our RSA private key containers include CRT params, since most crypto libraries support them. If at some point CRT params are not available, respective fields a filled with zeroes. */
/* struct members are ordered this way to avoid struct member alingment on different platforms */
#define DECLARE_RSA_PRIVATE_KEY(_KEY_SIZE_) \
	struct soter_rsa_priv_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t priv_exp[RSA_BYTE_SIZE(_KEY_SIZE_)];		uint8_t p[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t q[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t dp[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t dq[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t qp[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t mod[RSA_BYTE_SIZE(_KEY_SIZE_)]; \
		uint32_t pub_exp; /* Network byte order */ \
	}; \
	\
	typedef struct soter_rsa_priv_key_##_KEY_SIZE_##_type soter_rsa_priv_key_##_KEY_SIZE_##_t

#define DECLARE_RSA_KEY(_KEY_SIZE_) \
	DECLARE_RSA_PUBLIC_KEY(_KEY_SIZE_); \
	DECLARE_RSA_PRIVATE_KEY(_KEY_SIZE_)
/* We support 1024, 2048, 4096, 8192 RSA keys */
DECLARE_RSA_KEY(1024);
DECLARE_RSA_KEY(2048);
DECLARE_RSA_KEY(4096);
DECLARE_RSA_KEY(8192);

/* This is considered internal API */
typedef void soter_engine_specific_rsa_key_t;

soter_status_t soter_rsa_pub_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_rsa_key_t **engine_key);
soter_status_t soter_rsa_priv_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_rsa_key_t **engine_key);
soter_status_t soter_engine_specific_to_rsa_priv_key(const soter_engine_specific_rsa_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length);
soter_status_t soter_engine_specific_to_rsa_pub_key(const soter_engine_specific_rsa_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length);
#endif /* SOTER_RSA_KEY_H */
