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
#include <memory>

#include <boost/asio.hpp>

#include <themispp/secure_session.hpp>

#include "shared_keys.hpp"

using boost::asio::ip::tcp;

// You need to define a "callback object" like this which will identify
// peers and provide their public keys. Typically you read public keys
// from some persistent storage (e.g., from a file or database).
// Here for the sake of example we use pre-shared fixed keys.
class session_callbacks : public themispp::secure_session_callback_interface_t {
public:
    std::vector<uint8_t> get_pub_key_by_id(const std::vector<uint8_t>& id)
    {
        if (id == client_id) {
            return client_public_key;
        }
        if (id == server_id) {
            return server_public_key;
        }
        // Return an empty key if you don't recognise the peer.
        // This will cause Secure Session to abort.
        return std::vector<uint8_t>();
    }
};

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

    // Initialise Secure Session for the client
#if __cplusplus >= 201103L
    themispp::secure_session_t session(client_id, client_private_key,
                                       std::make_shared<session_callbacks>());
#else
    session_callbacks callbacks;
    themispp::secure_session_t session(client_id, client_private_key, &callbacks);
#endif

    // Negotiate Secure Session until the connection is established.
    // The client initiates connection and sends the first message.
    std::vector<uint8_t> negotiation_data = session.init();
    while (!session.is_established()) {
        // Send negotiation message to the server
        boost::asio::write(socket, boost::asio::buffer(negotiation_data), error);
        if (error) {
            throw boost::system::system_error(error);
        }

        // Receive negotiation reply from the server
        negotiation_data.resize(max_length);
        size_t reply_length = socket.read_some(boost::asio::buffer(negotiation_data), error);
        if (error) {
            throw boost::system::system_error(error);
        }
        negotiation_data.resize(reply_length);

        // Pass the reply to Secure Session, get back reply to that reply
        negotiation_data = session.unwrap(negotiation_data);
    }
    // The session is established now and we may exchange messages with the server

    // Read message from the user
    std::cout << "Enter message:" << std::endl;
    std::string request;
    std::getline(std::cin, request);

    // Encrypt the message for sending with Secure Session
    std::vector<uint8_t> request_bytes(request.begin(), request.end());
    std::vector<uint8_t> request_message = session.wrap(request_bytes);

    // Send encrypted message to the server
    boost::asio::write(socket, boost::asio::buffer(request_message), error);
    if (error) {
        throw boost::system::system_error(error);
    }

    // Receive encrypted reply from the server
    std::vector<uint8_t> reply_message(max_length);
    size_t reply_length = socket.read_some(boost::asio::buffer(reply_message), error);
    if (error) {
        throw boost::system::system_error(error);
    }
    reply_message.resize(reply_length);

    // Decrypt reply
    std::vector<uint8_t> reply_bytes = session.unwrap(reply_message);
    std::string reply(reply_bytes.begin(), reply_bytes.end());

    std::cout << "Client received a reply: " << reply << std::endl;

    return 0;
}
