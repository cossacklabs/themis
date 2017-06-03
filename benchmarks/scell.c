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

#include <themis/themis.h>

#include <string.h>
#include <assert.h>

#define PASSWORD "Benchmark Password"

int main(int argc, char* argv[]){
  if(argc != 2){
    return -1;
  }
  size_t data_length = atoi(argv[1]);
  uint8_t* data=malloc(data_length);
  assert(data);

  if(THEMIS_SUCCESS!=soter_rand(data, data_length) ){
    free(data);
    return -2;
  }

  size_t enc_data_length=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_cell_encrypt_seal(PASSWORD, strlen(PASSWORD), NULL, 0, data, data_length, NULL, &enc_data_length)){
    free(data);
    return -3;
  }
  uint8_t* enc_data = malloc(enc_data_length);
  assert(enc_data);

  if(THEMIS_SUCCESS!=themis_secure_cell_encrypt_seal(PASSWORD, strlen(PASSWORD), NULL, 0, data, data_length, enc_data, &enc_data_length)){
    free(data);
    free(enc_data);
    return -4;
  }
  free(data);

  size_t dec_data_len=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_cell_decrypt_seal(PASSWORD, strlen(PASSWORD), NULL, 0, enc_data, enc_data_length, NULL, &dec_data_len)){
    free(enc_data);
    return -3;
  }
  uint8_t* dec_data = malloc(dec_data_len);
  assert(dec_data);

  if(THEMIS_SUCCESS!=themis_secure_cell_decrypt_seal(PASSWORD, strlen(PASSWORD), NULL, 0, enc_data, enc_data_length, dec_data, &dec_data_len)){
    free(dec_data);
    free(enc_data);
    return -4;
  }
  free(dec_data);
  free(enc_data);
  return 0;
}
