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


#ifndef SOTER_KEYS_DB_H
#define SOTER_KEYS_DB_H

typedef struct soter_keys_db_type soter_keys_db_t;

uint32_t soter_keys_db_get_key(soter_keys_db_t* db, const uint8_t* id, const size_t id_length, uint8_t* key, size_t* key_length);

#endif //SOTER_KEY_DB_H
