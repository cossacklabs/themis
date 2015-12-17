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
#include <queue>
#include <algorithm>
#include <boost/thread/thread.hpp>
#include <themispp/secure_session.hpp>



namespace themispp{
  namespace secure_session_test{

    const uint8_t server_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x6c, 0x07, 0x99, 0x10, 0x00, 0x49, 0xbe, 0x76, 0x15, 0x5e, 0xeb, 0x8b, 0x68, 0xb5, 0x7f, 0x0f, 0xc4, 0xfd, 0xb5, 0x16, 0x41, 0x74, 0x7c, 0xf4, 0xe3, 0xed, 0xd7, 0xaf, 0x02, 0x7e, 0x95, 0x7d, 0xcf, 0x6d, 0x9b, 0x4e, 0x4a};
    const uint8_t server_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x16, 0x2d, 0x68, 0x68, 0x03, 0x96, 0x1a, 0xcd, 0xd1, 0x5b, 0x01, 0xea, 0x18, 0xb1, 0x63, 0x83, 0x2b, 0xe6, 0x85, 0x8d, 0x5e, 0xbf, 0xd2, 0xb0, 0xc2, 0xe1, 0x2a, 0xa3, 0x24, 0xeb, 0x37, 0xd3, 0xe7, 0xc7, 0xa2, 0xab, 0xdd};

    const uint8_t client_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xf2, 0x6a, 0x06, 0x8c, 0x00, 0x5b, 0xd7, 0x35, 0x5c, 0x53, 0x41, 0x89, 0xcd, 0x08, 0xa2, 0x30, 0xd2, 0x0b, 0x32, 0x4c, 0x74, 0x87, 0xc9, 0x0c, 0x2e, 0x9c, 0x86, 0xbf, 0x40, 0xf6, 0xea, 0xac, 0x8a, 0x75, 0x63, 0x67, 0xbc};
    const uint8_t client_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xba, 0x94, 0x97, 0xf7, 0x03, 0x06, 0x4f, 0x4c, 0xe0, 0x82, 0x74, 0xe3, 0xe9, 0x23, 0xa3, 0xac, 0x57, 0x1f, 0x1e, 0xf8, 0xf0, 0x66, 0x85, 0xee, 0xa3, 0x4c, 0x4f, 0xd5, 0xb9, 0x85, 0xc2, 0xde, 0x39, 0x49, 0x8d, 0x99, 0xe1};

    std::queue<std::vector<uint8_t> > qsc;
    std::queue<std::vector<uint8_t> > qcs;

    class transport{
    public:
      transport(std::queue<std::vector<uint8_t> >& to_read, std::queue<std::vector<uint8_t> >& to_write):
	_tr(to_read),
	_tw(to_write){}

      const std::vector<uint8_t> read(){
	while(_tr.empty()){sleep(1);}
	std::vector<uint8_t> a=_tr.front();
	_tr.pop();
	return a;
      }

      void write(const std::vector<uint8_t>& data){
	_tw.push(data);
      }

    private:
      std::queue<std::vector<uint8_t> >& _tr;
      std::queue<std::vector<uint8_t> >& _tw;
    };


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


    void client_func(){
      std::string mes("the test message");
      callback callbacks;
      transport tr(qsc, qcs);
      themispp::secure_session_t client("client", std::vector<uint8_t>(client_priv, client_priv+sizeof(client_priv)), &callbacks);
      std::vector<uint8_t> msg=client.init();
      while(!client.is_established()){
	tr.write(msg);
	msg=client.unwrap(tr.read());
      }
      tr.write(client.wrap(std::vector<uint8_t>(mes.c_str(), mes.c_str()+mes.length()+1)));
      sput_fail_unless(strcmp(mes.c_str(), (const char*)(&(client.unwrap(tr.read()))[0]))==0, "client part", __LINE__);
    }

    void server_func(){
      std::string mes("the test message");
      callback callbacks;
      transport tr(qcs, qsc);
      themispp::secure_session_t server("server", std::vector<uint8_t>(server_priv, server_priv+sizeof(server_priv)), &callbacks);
      while(!server.is_established()){
	tr.write(server.unwrap(tr.read()));
      } 
      std::vector<uint8_t> a=server.unwrap(tr.read());
      sput_fail_unless(strcmp(mes.c_str(), (const char*)(&a[0]))==0, "server part", __LINE__);
      tr.write(server.wrap(a));
    }

    void secure_session_test(){
      boost::thread client_thread(&client_func);
      boost::thread server_thread(&server_func);
      client_thread.join();
      server_thread.join();
    }
    
    int run_secure_session_test(){
      sput_enter_suite("ThemisPP secure session test");
      sput_run_test(secure_session_test, "secure_session_test", __FILE__);
      return 0;
    }
  }
}
#endif
