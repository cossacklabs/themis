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

#ifndef SOTER_EC_P256_M31_KEY_H
#define SOTER_EC_P256_M31_KEY_H

#include <soter/soter_container.h>
#include <soter/soter.h>

#include <bearssl.h>

#include <arpa/inet.h>

/** private key header part */
#define SOTER_EC_P256_M31_PRIV_KEY_TAG "RE12"
/** public key header part */
#define SOTER_EC_P256_M31_PUB_KEY_TAG "UE12"

#define SOTER_EC_P256_M31_PRIV_SIZE 32
#define SOTER_EC_P256_M31_PUB_SIZE 65

struct soter_ec_p256_m31_pub_key_type{
	soter_container_hdr_t hdr;
	br_ec_public_key impl;
	uint8_t key[SOTER_EC_P256_M31_PUB_SIZE];
	};

typedef struct soter_ec_p256_m31_pub_key_type soter_ec_p256_m31_pub_key_t;

soter_status_t soter_ec_p256_m31_pub_key_to_byte_array(soter_ec_p256_m31_pub_key_t* from, uint8_t* to, size_t* to_len);

soter_status_t soter_ec_p256_m31_byte_array_to_pub_key(const uint8_t* from, const size_t from_len, soter_ec_p256_m31_pub_key_t* to);

struct soter_ec_p256_m31_priv_key_type{
	soter_container_hdr_t hdr;
	br_ec_private_key impl;
	uint8_t key[SOTER_EC_P256_M31_PRIV_SIZE];
};

typedef struct soter_ec_p256_m31_priv_key_type soter_ec_p256_m31_priv_key_t;

soter_status_t soter_ec_p256_m31_priv_key_to_byte_array(soter_ec_p256_m31_priv_key_t* from, uint8_t* to, size_t* to_len);

soter_status_t soter_ec_p256_m31_byte_array_to_priv_key(const uint8_t* from, const size_t from_len, soter_ec_p256_m31_priv_key_t* to);

#endif /* SOTER_EC_P256_M31_KEY_H */
