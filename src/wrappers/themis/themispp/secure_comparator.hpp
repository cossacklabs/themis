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

#ifndef THEMISPP_SECURE_COMPARATOR_HPP_
#define THEMISPP_SECURE_COMPARATOR_HPP_

#include <cstring>
#include <vector>
#include <themis/themis.h>
#include "exception.hpp"

#define THEMISPP_SECURE_SESSION_MAX_MESSAGE_SIZE 2048
namespace themispp{

  class secure_comparator_t{
  public:
    typedef std::vector<uint8_t> data_t; 
    
    secure_comparator_t(const data_t& shared_secret):
      comparator_(NULL){
      res_.reserve(512);
      comparator_=secure_comparator_create();
      if(!comparator_)
	throw themispp::exception_t("Secure Comparator failed creating");
      if(THEMIS_SUCCESS!=secure_comparator_append_secret(comparator_, &shared_secret[0], shared_secret.size()))
	throw themispp::exception_t("Secure Comparator failed appending secret");	
    }

    virtual ~secure_comparator_t(){
      secure_comparator_destroy(comparator_);
    }

    const data_t& init(){
      size_t data_length=0;
      if(THEMIS_BUFFER_TOO_SMALL!=secure_comparator_begin_compare(comparator_, NULL, &data_length))
	throw themispp::exception_t("Secure Comparator failed making initialisation message");
      res_.resize(data_length);
      if(THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER!=secure_comparator_begin_compare(comparator_, &res_[0], &data_length))
	throw themispp::exception_t("Secure Comparator failed making initialisation message");
      return res_;
    }

    const data_t& proceed(const std::vector<uint8_t>& data){
      size_t res_data_length=0;
      if(THEMIS_BUFFER_TOO_SMALL!=secure_comparator_proceed_compare(comparator_, &data[0], data.size(), NULL, &res_data_length))
	throw themispp::exception_t("Secure Comparator failed proceeding message");
      res_.resize(res_data_length);
      themis_status_t res=secure_comparator_proceed_compare(comparator_, &data[0], data.size(), &res_[0], &res_data_length);
      if(THEMIS_SCOMPARE_SEND_OUTPUT_TO_PEER!=res && THEMIS_SUCCESS!=res)
	throw themispp::exception_t("Secure Comparator failed proceeding message");
      return res_;
    }
    
    const bool get(){
      return (THEMIS_SCOMPARE_MATCH == secure_comparator_get_result(comparator_))?true:false;
    }
  private:
    ::secure_comparator_t* comparator_;
    std::vector<uint8_t> res_;
  };
}// ns themis

#endif
