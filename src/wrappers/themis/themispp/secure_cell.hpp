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

#ifndef THEMISPP_SECURE_CELL_
#define THEMISPP_SECURE_CELL_

#include <vector>
#include <themis/themis.h>
#include "exception.hpp"

namespace themispp{

  class secure_cell_t{
  public:
    typedef std::vector<uint8_t> data_t;
    
    secure_cell_t(data_t::const_iterator password_begin, data_t::const_iterator password_end):
      _password(password_begin, password_end),
      _res(0){}

    secure_cell_t(const data_t& password):
      _password(password.begin(), password.end()),
      _res(0){}
    
    virtual const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end,  data_t::const_iterator context_begin, data_t::const_iterator context_end)=0;
    virtual const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end,  data_t::const_iterator context_begin, data_t::const_iterator context_end)=0;

    const data_t& encrypt(const data_t& data, const data_t& context){
      return encrypt(data.begin(), data.end(), context.begin(), context.end());
    }

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, const data_t& context){
      return encrypt(data_begin, data_end, context.begin(), context.end());
    }

    const data_t& encrypt(const data_t& data, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      return encrypt(data.begin(), data.end(), context_begin, context_end);
    }

    const data_t& decrypt(const data_t& data, const data_t& context){
      return decrypt(data.begin(), data.end(), context.begin(), context.end());
    }

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, const data_t& context){
      return decrypt(data_begin, data_end, context.begin(), context.end());
    }
    
    const data_t& decrypt(const data_t& data, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      return decrypt(data.begin(), data.end(), context_begin, context_end);
    }

  protected:
    data_t _password;
    data_t _res;
  };
    
  class secure_cell_optional_context_t: public secure_cell_t{
  public:
    secure_cell_optional_context_t(data_t::const_iterator password_begin, data_t::const_iterator password_end):
      secure_cell_t(password_begin, password_end){}
      
    secure_cell_optional_context_t(const data_t& password):
      secure_cell_t(password){}
    
    virtual const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      data_t context(0);
      return secure_cell_t::encrypt(data_begin, data_end, context);
    }

    virtual const data_t& encrypt(const data_t& data){
      return encrypt(data.begin(), data.end());
    }
    using secure_cell_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end){
      data_t context(0);
      return secure_cell_t::decrypt(data_begin, data_end, context);
    }

    const data_t& decrypt(const data_t data){
      return decrypt(data.begin(), data.end());
    }
    using secure_cell_t::decrypt;
  };
    
  class secure_cell_seal_t: public secure_cell_optional_context_t{
  public:
    secure_cell_seal_t(data_t::const_iterator password_begin, data_t::const_iterator password_end):
      secure_cell_optional_context_t(password_begin, password_end){}

    secure_cell_seal_t(const data_t& password):
      secure_cell_optional_context_t(password){}

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end,  data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t encrypted_length=0;
      if(themis_secure_cell_encrypt_seal(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Seal) failed encrypting");
      _res.resize(encrypted_length);
      if(themis_secure_cell_encrypt_seal(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_res[0], &encrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Seal) failed encrypting");
      return _res;
    }
    using secure_cell_optional_context_t::encrypt;
    
    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end,  data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t decrypted_length=0;
      if(themis_secure_cell_decrypt_seal(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Seal) failed decrypting");
      _res.resize(decrypted_length);
      if(themis_secure_cell_decrypt_seal(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_res[0], &decrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Seal) failed decrypting");
      return _res;
    }
    using secure_cell_optional_context_t::decrypt;
  };

  class secure_cell_token_protect_t: public secure_cell_optional_context_t{
  public:
    secure_cell_token_protect_t(data_t::const_iterator password_begin, data_t::const_iterator password_end):
      secure_cell_optional_context_t(password_begin, password_end),
      _token(0){}

    secure_cell_token_protect_t(const data_t& password):
      secure_cell_optional_context_t(password),
      _token(0){}

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t encrypted_length=0;
      size_t token_length=0;
      if(themis_secure_cell_encrypt_token_protect(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, NULL, &token_length, NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Token Protect) failed encrypting");
      _res.resize(encrypted_length);
      _token.resize(token_length);
      if(themis_secure_cell_encrypt_token_protect(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_token[0], &token_length, &_res[0], &encrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Token Protect) failed encrypting");
      return _res;
    }
    using secure_cell_optional_context_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t decrypted_length=0;
      if(themis_secure_cell_decrypt_token_protect(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_token[0], _token.size(), NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Token Protect) failed decrypting");
      _res.resize(decrypted_length);
      if(themis_secure_cell_decrypt_token_protect(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_token[0], _token.size(), &_res[0], &decrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Token Protect) failed decrypting");
      return _res;
    }

    using secure_cell_optional_context_t::decrypt;

    const data_t& get_token(){return _token;}
    void set_token(const data_t& token){_token=token;}
  protected:
    data_t _token;
  };

  class secure_cell_context_imprint_t: public secure_cell_t{
  public:
    secure_cell_context_imprint_t(const data_t& password):
      secure_cell_t(password){}

    const data_t& encrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t encrypted_length=0;
      if(themis_secure_cell_encrypt_context_imprint(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, NULL, &encrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Context Imprint) failed encrypting");
      _res.resize(encrypted_length);
      if(themis_secure_cell_encrypt_context_imprint(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_res[0], &encrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Context Imprint) failed encrypting");
      return _res;
    }
    using secure_cell_t::encrypt;

    const data_t& decrypt(data_t::const_iterator data_begin, data_t::const_iterator data_end, data_t::const_iterator context_begin, data_t::const_iterator context_end){
      size_t decrypted_length=0;
      if(themis_secure_cell_decrypt_context_imprint(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, NULL, &decrypted_length)!=THEMIS_BUFFER_TOO_SMALL)
	throw themispp::exception_t("Secure Cell (Context Imprint) failed decrypting");
      _res.resize(decrypted_length);
      if(themis_secure_cell_decrypt_context_imprint(&_password[0], _password.size(), &(*context_begin), context_end-context_begin, &(*data_begin), data_end-data_begin, &_res[0], &decrypted_length)!=THEMIS_SUCCESS)
	throw themispp::exception_t("Secure Cell (Context Imprint) failed decrypting");
      return _res;
    }
    using secure_cell_t::decrypt;
  };
}//themispp ns

#endif