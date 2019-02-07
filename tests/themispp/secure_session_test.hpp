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

#ifndef THEMISPP_SECURE_SESSION_TEST_HPP_
#define THEMISPP_SECURE_SESSION_TEST_HPP_

#include <common/sput.h>
#include <string.h>
#include <themispp/secure_session.hpp>

#if __cplusplus >= 201103L
#include <unordered_map>
#endif

namespace themispp{
  namespace secure_session_test{

    const uint8_t server_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x6c, 0x07, 0x99, 0x10, 0x00, 0x49, 0xbe, 0x76, 0x15, 0x5e, 0xeb, 0x8b, 0x68, 0xb5, 0x7f, 0x0f, 0xc4, 0xfd, 0xb5, 0x16, 0x41, 0x74, 0x7c, 0xf4, 0xe3, 0xed, 0xd7, 0xaf, 0x02, 0x7e, 0x95, 0x7d, 0xcf, 0x6d, 0x9b, 0x4e, 0x4a};
    const uint8_t server_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x16, 0x2d, 0x68, 0x68, 0x03, 0x96, 0x1a, 0xcd, 0xd1, 0x5b, 0x01, 0xea, 0x18, 0xb1, 0x63, 0x83, 0x2b, 0xe6, 0x85, 0x8d, 0x5e, 0xbf, 0xd2, 0xb0, 0xc2, 0xe1, 0x2a, 0xa3, 0x24, 0xeb, 0x37, 0xd3, 0xe7, 0xc7, 0xa2, 0xab, 0xdd};

    const uint8_t client_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xf2, 0x6a, 0x06, 0x8c, 0x00, 0x5b, 0xd7, 0x35, 0x5c, 0x53, 0x41, 0x89, 0xcd, 0x08, 0xa2, 0x30, 0xd2, 0x0b, 0x32, 0x4c, 0x74, 0x87, 0xc9, 0x0c, 0x2e, 0x9c, 0x86, 0xbf, 0x40, 0xf6, 0xea, 0xac, 0x8a, 0x75, 0x63, 0x67, 0xbc};
    const uint8_t client_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xba, 0x94, 0x97, 0xf7, 0x03, 0x06, 0x4f, 0x4c, 0xe0, 0x82, 0x74, 0xe3, 0xe9, 0x23, 0xa3, 0xac, 0x57, 0x1f, 0x1e, 0xf8, 0xf0, 0x66, 0x85, 0xee, 0xa3, 0x4c, 0x4f, 0xd5, 0xb9, 0x85, 0xc2, 0xde, 0x39, 0x49, 0x8d, 0x99, 0xe1};


    class callback: public themispp::secure_session_callback_interface_t{
    public:
      const std::vector<uint8_t> get_pub_key_by_id(const std::vector<uint8_t>& id){
	std::string id_str(&id[0], &id[0]+id.size());
	if(id_str=="client")
	  return std::vector<uint8_t>(client_pub, client_pub+sizeof(client_pub));
	else if(id_str=="server")
	  return std::vector<uint8_t>(server_pub, server_pub+sizeof(server_pub));
	return std::vector<uint8_t>(0);
      }
    };

    void secure_session_test(){
      std::string mes("the test message");
      callback client_callbacks;
      std::string client_id("client");
      std::string server_id("server");
      
      themispp::secure_session_t client(std::vector<uint8_t>(client_id.c_str(), client_id.c_str()+client_id.length()), std::vector<uint8_t>(client_priv, client_priv+sizeof(client_priv)), &client_callbacks);

      callback server_callbacks;
      themispp::secure_session_t server(std::vector<uint8_t>(server_id.c_str(), server_id.c_str()+server_id.length()), std::vector<uint8_t>(server_priv, server_priv+sizeof(server_priv)), &server_callbacks);

      std::vector<uint8_t> control_msg1=client.init();
      std::vector<uint8_t> control_msg2=server.unwrap(control_msg1);
      std::vector<uint8_t> control_msg3=client.unwrap(control_msg2);
      std::vector<uint8_t> control_msg4=server.unwrap(control_msg3);
      std::vector<uint8_t> control_msg5=client.unwrap(control_msg4);
      sput_fail_unless(server.is_established(), "server ready", __LINE__);
      sput_fail_unless(client.is_established(), "client ready", __LINE__);

      std::vector<uint8_t> msg1=client.wrap(std::vector<uint8_t>(mes.c_str(), mes.c_str()+mes.length()+1));
      std::vector<uint8_t> msg2=server.unwrap(msg1);
      sput_fail_unless(strcmp(mes.c_str(), (const char*)(&msg2[0]))==0, "server get message", __LINE__);

      std::vector<uint8_t> msg3=server.wrap(std::vector<uint8_t>(mes.c_str(), mes.c_str()+mes.length()+1));
      std::vector<uint8_t> msg4=client.unwrap(msg3);
      sput_fail_unless(strcmp(mes.c_str(), (const char*)(&msg4[0]))==0, "client get message", __LINE__);
    }

    void secure_session_uninitialized(){
      themispp::secure_session_t client;

      bool init_throws=false;
      try{
        client.init();
      }
      catch(const themispp::exception_t&){
        init_throws=true;
      }
      sput_fail_unless(init_throws, "using uninitialized session", __LINE__);
    }

#if __cplusplus >= 201103L
    void secure_session_moved(){
      callback client_callbacks;
      std::string client_id("client");
      themispp::secure_session_t client(std::vector<uint8_t>(client_id.c_str(), client_id.c_str()+client_id.length()), std::vector<uint8_t>(client_priv, client_priv+sizeof(client_priv)), &client_callbacks);

      std::unordered_map<std::string, themispp::secure_session_t> clients;
      clients.emplace(client_id, std::move(client));

      bool init_throws=false;
      try{
        client.init();
      }
      catch(const themispp::exception_t&){
        init_throws=true;
      }
      sput_fail_unless(init_throws, "deny using moved-out session", __LINE__);

      std::vector<uint8_t> control_msg1=clients.at(client_id).init();
      sput_fail_if(control_msg1.empty(), "initializing moved session", __LINE__);

      client=std::move(clients.at(client_id));

      sput_fail_if(client.is_established(), "using moved session again", __LINE__);
    }
#else
    void secure_session_moved(){
      sput_fail_if(false, "move semantics (disabled for C++03)", __LINE__);
    }
#endif

    int run_secure_session_test(){
      sput_enter_suite("ThemisPP secure session test");
      sput_run_test(secure_session_test, "secure_session_test", __FILE__);
      sput_run_test(secure_session_uninitialized, "secure_session_uninitialized", __FILE__);
      sput_run_test(secure_session_moved, "secure_session_moved", __FILE__);
      return 0;
    }
  }
}
#endif
