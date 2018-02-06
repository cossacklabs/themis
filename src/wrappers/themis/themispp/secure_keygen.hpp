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

#ifndef THEMISPP_SECURE_KEYGEN_HPP_
#define THEMISPP_SECURE_KEYGEN_HPP_

#include <cstring>
#include <vector>
#include "themis/themis.h"
#include "exception.hpp"

#define MAX_KEY_LENGTH 10*1024

namespace themispp{

  enum asym_algs {EC, RSA};


  template <asym_algs alg_t_p, size_t max_key_length_t_p = MAX_KEY_LENGTH>
  class secure_key_pair_generator_t{
  public:
    secure_key_pair_generator_t():
      private_key(max_key_length_t_p),
      public_key(max_key_length_t_p){
      gen();
    }
    
    void gen(){
      size_t private_key_length=max_key_length_t_p;
      size_t public_key_length=max_key_length_t_p;
      switch(alg_t_p){
      case EC:
	if(themis_gen_ec_key_pair(&private_key[0], &private_key_length, &public_key[0], &public_key_length)!=THEMIS_SUCCESS)
	  throw themispp::exception_t("Themis failed generating EC KeyPair");
	break;
      case RSA:
	if(themis_gen_rsa_key_pair(&private_key[0], &private_key_length, &public_key[0], &public_key_length)!=THEMIS_SUCCESS)
	  throw themispp::exception_t("Themis failed generating RSA KeyPair");
	break;
      default:
	throw themispp::exception_t("Themis failed generating KeyPair, unsupported algorithm");
      }
      private_key.resize(private_key_length);
      public_key.resize(public_key_length);
    }
      
    const std::vector<uint8_t>& get_priv(){return private_key;}
    const std::vector<uint8_t>& get_pub(){return public_key;}

  private:
    std::vector<uint8_t> private_key;
    std::vector<uint8_t> public_key;
  };

}// ns themis

#endif
