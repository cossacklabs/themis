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

#ifndef THEMISPP_MESSAGE_TEST_HPP
#define THEMISPP_MESSAGE_TEST_HPP

#include <common/sput.h>
#include <iostream>
#include <themispp/secure_keygen.hpp>
#include <themispp/secure_message.hpp>


namespace themispp {
    namespace secure_message_test {
        static std::string message1 = "this is test message1 for Secure Message test";
        static std::string message2 = "this is test message2 for Secure Message test";


        template <asym_algs alg_t_p>
        int secure_message_test_() {
            try {
                themispp::secure_key_pair_generator_t<alg_t_p> g;
                std::vector<uint8_t> private_key(g.get_priv());
                std::vector<uint8_t> public_key(g.get_pub());
                g.gen();
                std::vector<uint8_t> peer_private_key(g.get_priv());
                std::vector<uint8_t> peer_public_key(g.get_pub());
                g.gen();
                std::vector<uint8_t> intruder_private_key(g.get_priv());
                std::vector<uint8_t> intruder_public_key(g.get_pub());
                themispp::secure_message_t a(private_key, peer_public_key);
                themispp::secure_message_t b(peer_private_key, public_key);
                themispp::secure_message_t c(intruder_private_key, public_key);

                sput_fail_unless(private_key != peer_private_key, "generate two identical key pairs", __LINE__);
                std::vector<uint8_t> encrypted_message = a.encrypt(
                        std::vector<uint8_t>(message1.c_str(), message1.c_str() + message1.length()));
                std::vector<uint8_t> decrypted_message = b.decrypt(encrypted_message);
                sput_fail_unless(!message1.compare(std::string(decrypted_message.begin(), decrypted_message.end())),
                                 "decryption fail", __LINE__);
                try {
                    decrypted_message = c.decrypt(encrypted_message);
                    sput_fail_unless(false, "decryption fail", __LINE__);
                } catch (themispp::exception_t &e) {
                    sput_fail_unless(true, "decryption by intruder", __LINE__);
                }
            } catch (themispp::exception_t &e) {
                sput_fail_unless(false, e.what(), __LINE__);
            }
            return 0;
        }
        int secure_message_test_rsa() {
            return secure_message_test_<themispp::RSA>();
        }
        int secure_message_test_ec() {
            return secure_message_test_<themispp::EC>();
        }

        template <asym_algs alg_t_p>
        int secure_message_sign_verify_test() {
            try {
                themispp::secure_key_pair_generator_t<alg_t_p> g;
                std::vector<uint8_t> private_key(g.get_priv());
                std::vector<uint8_t> public_key(g.get_pub());
                g.gen();
                std::vector<uint8_t> peer_private_key(g.get_priv());
                std::vector<uint8_t> peer_public_key(g.get_pub());
                g.gen();
                std::vector<uint8_t> intruder_private_key(g.get_priv());
                std::vector<uint8_t> intruder_public_key(g.get_pub());
                themispp::secure_message_t a(private_key, peer_public_key);
                themispp::secure_message_t b(peer_private_key, public_key);
                themispp::secure_message_t c(intruder_private_key, public_key);

                sput_fail_unless(private_key != peer_private_key, "generate two identical key pairs", __LINE__);
                std::vector<uint8_t> a_signed_message = a.sign(
                        std::vector<uint8_t>(message1.c_str(), message1.c_str() + message1.length()));
                std::vector<uint8_t> verified_message = b.verify(a_signed_message);
                sput_fail_unless(!message1.compare(std::string(verified_message.begin(), verified_message.end())),
                                 "verification fail", __LINE__);
                // corrupt one byte
                a_signed_message[2]++;
                try {
                    verified_message = c.verify(a_signed_message);
                    sput_fail_unless(false, "verification fail", __LINE__);
                } catch (themispp::exception_t &e) {
                    sput_fail_unless(true, "verification by intruder", __LINE__);
                }
            } catch (themispp::exception_t &e) {
                sput_fail_unless(false, e.what(), __LINE__);
            }
            return 0;
        }

        int secure_message_sign_verify_test_rsa(){
            return secure_message_sign_verify_test<themispp::RSA>();
        }

        int secure_message_sign_verify_test_ec(){
            return secure_message_sign_verify_test<themispp::EC>();
        }

        void run_secure_message_test() {
            sput_enter_suite("ThemisPP secure message test");
            sput_run_test(secure_message_test_rsa, "secure_message_test_rsa", __FILE__);
            sput_run_test(secure_message_test_ec, "secure_message_test_ec", __FILE__);
            sput_run_test(secure_message_sign_verify_test_rsa, "secure_message_sign_verify_test_rsa", __FILE__);
            sput_run_test(secure_message_sign_verify_test_ec, "secure_message_sign_verify_test_ec", __FILE__);
        }
    }
}
#endif
