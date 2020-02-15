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
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

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

typedef boost::shared_ptr<tcp::socket> socket_ptr;

// Negotiate Secure Session connection to the client.
void negotiate_secure_session(themispp::secure_session_t& session, tcp::socket& socket)
{
    // The server expects to receive the first message from the client
    do {
        boost::system::error_code error;

        // Receive negotiation request from the server
        std::vector<uint8_t> negotiation_request(max_length);
        size_t request_length = socket.read_some(boost::asio::buffer(negotiation_request), error);
        if (error) {
            throw boost::system::system_error(error);
        }
        negotiation_request.resize(request_length);

        // Process the request in Secure Session, receive a reply
        std::vector<uint8_t> negotiation_reply = session.unwrap(negotiation_request);

        // Send reply to the client
        boost::asio::write(socket, boost::asio::buffer(negotiation_reply), error);
        if (error) {
            throw boost::system::system_error(error);
        }
    } while(!session.is_established());
}

// Receive and handle one request. Return true to continue serving.
bool serve_request(themispp::secure_session_t& session, tcp::socket& socket)
{
    boost::system::error_code error;

    // Receive encrypted reply from the client
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

    // Decrypt request with Secure Session
    std::vector<uint8_t> request_bytes = session.unwrap(request_encrypted);
    std::string request(request_bytes.begin(), request_bytes.end());

    std::cout << "Server received request: " << request << std::endl;

    // Encrypt echo reply with Secure Session
    std::vector<uint8_t> reply_encrypted = session.wrap(request_bytes);

    // Send reply to the client
    boost::asio::write(socket, boost::asio::buffer(reply_encrypted), error);
    if (error) {
        throw boost::system::system_error(error);
    }

    return true;
}

// Client session: init session, receive requests, handle them until done.
void session(socket_ptr socket)
{
    try {
        // Initialise Secure Session for the server
#if __cplusplus >= 201103L
        themispp::secure_session_t session(server_id, server_private_key,
                                           std::make_shared<session_callbacks>());
#else
        session_callbacks callbacks;
        themispp::secure_session_t session(server_id, server_private_key, &callbacks);
#endif
        negotiate_secure_session(session, *socket);

        while (serve_request(session, *socket)) {
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
