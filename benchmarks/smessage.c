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

#include <themis/themis.h>

#include <string.h>
#include <assert.h>

int main(int argc, char* argv[]){
  if(argc != 2){
    return 1;
  }

  uint8_t *sk1=NULL, *sk2=NULL, *pk1=NULL, *pk2=NULL;
  size_t sk1_len=0, sk2_len=0, pk1_len=0, pk2_len=0;

  if(THEMIS_BUFFER_TOO_SMALL!=themis_gen_key_pair(THEMIS_SECURE_ENCRYPTED_MESSAGE_DEFAULT_ALG, sk1, &sk1_len, pk1, &pk1_len)){
    return 2;
  }
  sk1 = malloc(sk1_len);
  assert(sk1);
  pk1 = malloc(pk1_len);
  assert(pk1);

  if(THEMIS_SUCCESS!=themis_gen_key_pair(THEMIS_SECURE_ENCRYPTED_MESSAGE_DEFAULT_ALG, sk1, &sk1_len, pk1, &pk1_len)){
    free(sk1);
    free(pk1);
    return 3;
  }

  if(THEMIS_BUFFER_TOO_SMALL!=themis_gen_key_pair(THEMIS_SECURE_ENCRYPTED_MESSAGE_DEFAULT_ALG, sk2, &sk2_len, pk2, &pk2_len)){
    free(sk1);
    free(pk1);
    return 4;
  }
  sk2 = malloc(sk2_len);
  assert(sk2);
  pk2 = malloc(pk2_len);
  assert(pk2);

  if(THEMIS_SUCCESS!=themis_gen_key_pair(THEMIS_SECURE_ENCRYPTED_MESSAGE_DEFAULT_ALG, sk2, &sk2_len, pk2, &pk2_len)){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    return 5;
  }

  size_t data_length = atoi(argv[1]);
  uint8_t* data=malloc(data_length);
  assert(data);

  if(THEMIS_SUCCESS!=soter_rand(data, data_length) ){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    free(data);
    return 6;
  }

  
  size_t enc_data_length=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_message_wrap(sk1, sk1_len, pk2, pk2_len, data, data_length, NULL, &enc_data_length)){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    free(data);
    return 7;
  }
  uint8_t* enc_data = malloc(enc_data_length);
  assert(enc_data);

  if(THEMIS_SUCCESS!=themis_secure_message_wrap(sk1, sk1_len, pk2, pk2_len, data, data_length, enc_data, &enc_data_length)){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    free(data);
    free(enc_data);
    return 8;
  }
  free(data);

  size_t dec_data_len=0;
  if(THEMIS_BUFFER_TOO_SMALL != themis_secure_message_unwrap(sk2, sk2_len, pk1, pk1_len, enc_data, enc_data_length, NULL, &dec_data_len)){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    free(enc_data);
    return 9;
  }
  uint8_t* dec_data = malloc(dec_data_len);
  assert(dec_data);

  if(THEMIS_SUCCESS != themis_secure_message_unwrap(sk2, sk2_len, pk1, pk1_len, enc_data, enc_data_length, dec_data, &dec_data_len)){
    free(sk1);
    free(pk1);
    free(sk2);
    free(pk2);
    free(dec_data);
    free(enc_data);
    return 10;
  }
  free(sk1);
  free(pk1);
  free(sk2);
  free(pk2);
  free(dec_data);
  free(enc_data);
  return 0;
}
