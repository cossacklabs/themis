/*
 * Copyright (c) 2017 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef SOTER_KEY_STORE_H
#define SOTER_KEY_STORE_H

#include <soter/soter_asym_ka.h>
#include "db.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct soter_crypter_type soter_crypter_t;

soter_crypter_t* soter_crypter_create(soter_keys_db_t* db);
soter_status_t soter_crypter_destroy(soter_crypter_t** ks);
soter_status_t soter_crypter_db_get_key(soter_crypter_t* ks, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length);


soter_asym_ka_t* soter_crypter_asym_ka_create(soter_crypter_t* crypter, const uint8_t* key, const size_t key_length);
soter_status_t soter_crypter_asym_ka_destroy(soter_crypter_t* crypter, soter_asym_ka_t* ka);
soter_status_t soter_crypter_asym_ka_derive(soter_crypter_t* crypter, soter_asym_ka_t* ka, const uint8_t* peer_key, const size_t peer_key_length, uint8_t* shared_secret, size_t* shared_secret_length);




#endif //KEY_STORE_H
