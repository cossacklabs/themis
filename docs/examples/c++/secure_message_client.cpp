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

#include <iostream>

#include <boost/asio.hpp>

#include <themispp/secure_message.hpp>

#include "shared_keys.hpp"

using boost::asio::ip::tcp;

const size_t max_length = 1024;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "usage:" << std::endl
                  << "     " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }
    const char* host = argv[1];
    const char* port = argv[2];

    // Initialise Secure Message with pre-shared keys
    themispp::secure_message_t smessage(client_private_key, server_public_key);

    boost::asio::io_service io_service;
    boost::system::error_code error;

    // Resolve server's address
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, port);
    tcp::resolver::iterator address_end;
    tcp::resolver::iterator address = resolver.resolve(query, error);
    if (error) {
        std::cerr << "failed to resolve " << host << ":" << port << std::endl;
        return 1;
    }

    // Connect to the server
    tcp::socket socket(io_service);
    while (address != address_end) {
        socket.connect(*address++, error);
        if (!error) {
            break;
        }
    }
    if (!socket.is_open()) {
        std::cerr << "failed to connect to " << host << ":" << port << std::endl;
        return 1;
    }

    // Read message from the user
    std::cout << "Enter message:" << std::endl;
    std::string request;
    std::getline(std::cin, request);

    // Encrypt message with Secure Message
    std::vector<uint8_t> request_bytes(request.begin(), request.end());
    std::vector<uint8_t> request_encrypted = smessage.encrypt(request_bytes);

    // Send encrypted message to the server
    boost::asio::write(socket, boost::asio::buffer(request_encrypted), error);
    if (error) {
        throw boost::system::system_error(error);
    }

    // Receive encrypted reply from the server
    std::vector<uint8_t> reply_encrypted(max_length);
    size_t reply_length = socket.read_some(boost::asio::buffer(reply_encrypted), error);
    if (error) {
        throw boost::system::system_error(error);
    }
    reply_encrypted.resize(reply_length);

    // Decrypt reply
    std::vector<uint8_t> reply_bytes = smessage.decrypt(reply_encrypted);
    std::string reply(reply_bytes.begin(), reply_bytes.end());

    std::cout << "Client received a reply: " << reply << std::endl;

    return 0;
}
