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

#ifndef SOTER_X25519_KEY_H
#define SOTER_X25519_KEY_H

#include <soter/soter_container.h>
#include <soter/soter.h>

#include <arpa/inet.h>

#include <sodium.h>

/** private key header part */
#define X25519_PRIV_KEY_TAG "RX20"
/** public key header part */
#define X25519_PUB_KEY_TAG "UX20"

#define X25519_PRIV_SIZE crypto_box_SECRETKEYBYTES
#define X25519_PUB_SIZE crypto_box_PUBLICKEYBYTES

struct soter_x25519_pub_key_type{
	soter_container_hdr_t hdr;
	uint8_t key[X25519_PUB_SIZE];
	};

typedef struct soter_x25519_pub_key_type soter_x25519_pub_key_t;

struct soter_x25519_priv_key_type{
	soter_container_hdr_t hdr;
	uint8_t key[X25519_PRIV_SIZE];
};

typedef struct soter_x25519_priv_key_type soter_x25519_priv_key_t;
#endif /* SOTER_X255519_KEY_H */
