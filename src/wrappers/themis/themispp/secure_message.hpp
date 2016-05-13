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

#ifndef THEMISPP_SECURE_MESSAGE_HPP_
#define THEMISPP_SECURE_MESSAGE_HPP_

#include "exception.hpp"
#include <themis/themis.h>
#include <vector>

namespace themispp {

  class secure_message_t
  {
  public:
    typedef std::vector<uint8_t> data_t;
    
    secure_message_t(data_t::const_iterator private_key_begin, data_t::const_iterator private_key_end, data_t::const_iterator peer_public_key_begin, data_t::const_iterator peer_public_key_end):
      private_key_(private_key_begin, private_key_end),
      peer_public_key_(peer_public_key_begin, peer_public_key_end){}

    secure_message_t(const data_t& private_key, data_t::const_iterator peer_public_key_begin, data_t::const_iterator peer_public_key_end):
      private_key_(private_key.begin(), private_key.end()),
      peer_public_key_(peer_public_key_begin, peer_public_key_end){}

    secure_message_t(data_t::const_iterator private_key_begin, data_t::const_iterator private_key_end, const data_t& peer_public_key):
      private_key_(private_key_begin, private_key_end),
      peer_public_key_(peer_public_key.begin(), peer_public_key.end()){}
    
    secure_message_t(const data_t& private_key, const data_t& peer_public_key):
      private_key_(private_key.begin(), private_key.end()),
      peer_public_key_(peer_public_key.begin(), peer_public_key.end()){}

    virtual ~secure_message_t(){}

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      size_t encrypted_data_length=0;
      if(themis_secure_message_wrap(&private_key_[0], private_key_.size(), &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, NULL, &encrypted_data_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Message failed encrypting");
      res_.resize(encrypted_data_length);
      if(themis_secure_message_wrap(&private_key_[0], private_key_.size(), &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, &res_[0], &encrypted_data_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Message failed encrypting");
      return res_;
    }

    const data_t& encrypt(const data_t& data){
      return encrypt(data.begin(), data.end());
    }

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      size_t decrypted_data_length=0;
      if(themis_secure_message_unwrap(&private_key_[0], private_key_.size(), &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, NULL, &decrypted_data_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Message failed decrypting");
      res_.resize(decrypted_data_length);
      if(themis_secure_message_unwrap(&private_key_[0], private_key_.size(), &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, &res_[0], &decrypted_data_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Message failed decrypting");
      return res_;
    }

    const data_t& decrypt(const data_t& data){
      return decrypt(data.begin(), data.end());
    }

    const data_t& sign(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      size_t encrypted_data_length=0;
      if(themis_secure_message_wrap(&private_key_[0], private_key_.size(), NULL, 0, &(*data_begin), data_end-data_begin, NULL, &encrypted_data_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Message failed singing");
      res_.resize(encrypted_data_length);
      if(themis_secure_message_wrap(&private_key_[0], private_key_.size(), NULL, 0, &(*data_begin), data_end-data_begin, &res_[0], &encrypted_data_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Message failed singing");
      return res_;
    }

    const data_t& sign(const data_t& data){
      return sign(data.begin(), data.end());
    }

    const data_t& verify(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      size_t decrypted_data_length=0;
      if(themis_secure_message_unwrap(NULL, 0, &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, NULL, &decrypted_data_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Message failed verifying");
      res_.resize(decrypted_data_length);
      if(themis_secure_message_unwrap(NULL, 0, &peer_public_key_[0], peer_public_key_.size(), &(*data_begin), data_end-data_begin, &res_[0], &decrypted_data_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Message failed verifying");
      return res_;
    }

    const data_t& verify(const data_t& data){
      return verify(data.begin(), data.end());
    }

  private:
    data_t private_key_;
    data_t peer_public_key_;
    data_t res_;
  private:
    secure_message_t(const secure_message_t&);
    secure_message_t& operator=(const secure_message_t&);
  };
} //end themispp

#endif /* THEMISPP_SECURE_MESSAGE_HPP_ */
