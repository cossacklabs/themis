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

#include <themispp/secure_message.hpp>

#include "shared_keys.hpp"

using boost::asio::ip::tcp;

const size_t max_length = 1024;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

// Receive and handle one request. Return true to continue serving.
bool serve_request(tcp::socket& socket)
{
    boost::system::error_code error;

    // Initialise Secure Message with pre-shared keys
    themispp::secure_message_t smessage(server_private_key, client_public_key);

    // Receive encrypted request from the client
    std::vector<uint8_t> request_encrypted(max_length);
    size_t request_length = socket.read_some(boost::asio::buffer(request_encrypted), error);
    if (error == boost::asio::error::eof) {
        // Connection closed cleanly by peer
        return false;
    }
    if (error) {
        throw boost::system::system_error(error);
    }
    request_encrypted.resize(request_length);

    // Decrypt request with Secure Message
    std::vector<uint8_t> request_bytes = smessage.decrypt(request_encrypted);
    std::string request(request_bytes.begin(), request_bytes.end());

    std::cout << "Server received request: " << request << std::endl;

    // Encrypt echo reply with Secure Message
    std::vector<uint8_t> reply_encrypted = smessage.encrypt(request_bytes);

    // Send encrypted reply to the client
    boost::asio::write(socket, boost::asio::buffer(reply_encrypted), error);
    if (error) {
        throw boost::system::system_error(error);
    }

    return true;
}

// Client session: receive requests, handle them until done.
void session(socket_ptr socket)
{
    try {
        while (serve_request(*socket)) {
            continue;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

// Server loop: accept connections, spawn threads to handle them.
void server(boost::asio::io_service& io_service, int port)
{
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v6(), port));
    for (;;) {
        socket_ptr socket(new tcp::socket(io_service));
        acceptor.accept(*socket);
        boost::thread thread(boost::bind(session, socket));
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "usage:" << std::endl
                  << "     " << argv[0] << " <listen-port>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);

    boost::asio::io_service io_service;

    server(io_service, port);

    return 0;
}
