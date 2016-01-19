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



#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string.h>
#include <themispp/secure_session.hpp>

const uint8_t client_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x00, 0xb2, 0x7f, 0x81, 0x00, 0x60, 0x9d, 0xe7, 0x7a, 0x39, 0x93, 0x68, 0xfc, 0x25, 0xd1, 0x79, 0x88, 0x6d, 0xfb, 0xf6, 0x19, 0x35, 0x53, 0x74, 0x10, 0xfc, 0x5b, 0x44, 0xe1, 0xf6, 0xf4, 0x4e, 0x59, 0x8d, 0x94, 0x99, 0x4f};
const uint8_t client_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x10, 0xf4, 0x68, 0x8c, 0x02, 0x1c, 0xd0, 0x3b, 0x20, 0x84, 0xf2, 0x7a, 0x38, 0xbc, 0xf6, 0x39, 0x74, 0xbf, 0xc3, 0x13, 0xae, 0xb1, 0x00, 0x26, 0x78, 0x07, 0xe1, 0x7f, 0x63, 0xce, 0xe0, 0xb8, 0xac, 0x02, 0x10, 0x40, 0x10};

const uint8_t server_priv[]={0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xd0, 0xfd, 0x93, 0xc6, 0x00, 0xae, 0x83, 0xb3, 0xef, 0xef, 0x06, 0x2c, 0x9d, 0x76, 0x63, 0xf2, 0x50, 0xd8, 0xac, 0x32, 0x6e, 0x73, 0x96, 0x60, 0x53, 0x77, 0x51, 0xe4, 0x34, 0x26, 0x7c, 0xf2, 0x9f, 0xb6, 0x96, 0xeb, 0xd8};
const uint8_t server_pub[]={0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0xa5, 0xb3, 0x9b, 0x9d, 0x03, 0xcd, 0x34, 0xc5, 0xc1, 0x95, 0x6a, 0xb2, 0x50, 0x43, 0xf1, 0x4f, 0xe5, 0x88, 0x3a, 0x0f, 0xb1, 0x11, 0x8c, 0x35, 0x81, 0x82, 0xe6, 0x9e, 0x5c, 0x5a, 0x3e, 0x14, 0x06, 0xc5, 0xb3, 0x7d, 0xdd};

using boost::asio::ip::tcp;

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

const int max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

void session(socket_ptr sock)
{
  try
  {
    std::string server_id("server");
    callback callbacks;
    themispp::secure_session_t session(std::vector<uint8_t>(server_id.c_str(), server_id.c_str()+server_id.length()), std::vector<uint8_t>(server_priv, server_priv+sizeof(server_priv)), &callbacks);
    do{
      boost::system::error_code error;
      char data[max_length];
      size_t length = sock->read_some(boost::asio::buffer(data), error);
      std::vector<uint8_t> d=session.unwrap(std::vector<uint8_t>(data, data+length));
      boost::asio::write(*sock, boost::asio::buffer(&d[0], d.size()));
    }while(!session.is_established());

    for (;;)
    {
      char data[max_length];
      boost::system::error_code error;
      size_t length = sock->read_some(boost::asio::buffer(data), error);
      std::cout<<length<<std::endl;      
      std::vector<uint8_t> d=session.unwrap(std::vector<uint8_t>(data, data+length));
      std::vector<uint8_t> d2=session.wrap(d);
      boost::asio::write(*sock, boost::asio::buffer(&d2[0], d2.size()));
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
  }
}

void server(boost::asio::io_service& io_service, short port)
{
  tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
  for (;;)
  {
    socket_ptr sock(new tcp::socket(io_service));
    a.accept(*sock);
    boost::thread t(boost::bind(session, sock));
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: secure_session_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server(io_service, atoi(argv[1]));
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
