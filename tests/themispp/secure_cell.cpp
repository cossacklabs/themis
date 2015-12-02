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

#include <common/sput.h>
#include <themispp/secure_cell.hpp>

static std::string password1("password1");
static std::string password2("password2");

static std::string message1("secure cell test message1 (c)Cossacklabs");
static std::string message2("secure cell test message2 (c)Cossacklabs");

static std::string context1("secure cell test context1 message");
static std::string context2("secure cell test context2 message");

static void secure_cell_seal_test(){
  //construction
  std::vector<uint8_t> pass1_vector(password1.c_str(), password1.c_str()+password1.length());
  std::vector<uint8_t> pass2_vector(password2.c_str(), password2.c_str()+password2.length());
  std::vector<uint8_t> mess1_vector(message1.c_str(),  message1.c_str()+message1.length());
  std::vector<uint8_t> mess2_vector(message2.c_str(), message2.c_str()+message2.length());
  std::vector<uint8_t> cont1_vector(context1.c_str(), context1.c_str()+context1.length());
  std::vector<uint8_t> cont2_vector(context2.c_str(), context2.c_str()+context2.length());
  themispp::secure_cell_seal a1(pass1_vector.begin(), pass1_vector.end());
  themispp::secure_cell_seal a2(pass1_vector);
  themispp::secure_cell_seal b1(pass2_vector);

  sput_fail_unless(a1.encrypt(mess1_vector) != a2.encrypt(mess1_vector), "two different seal encryption with same keys can't be the same", __LINE__);
  sput_fail_unless(a1.encrypt(mess1_vector) != b1.encrypt(mess1_vector), "two different seal encryption with different keys can't be the same", __LINE__);
  sput_fail_unless(a1.encrypt(mess1_vector) != a2.encrypt(mess2_vector), "two different seal encryption with different messages can't be the same", __LINE__);
  std::vector<uint8_t> r1=a1.encrypt(mess1_vector);
  std::vector<uint8_t> r2=a2.encrypt(mess1_vector.begin(), mess1_vector.end());
  std::vector<uint8_t> r3=b1.encrypt(mess1_vector.begin(), mess1_vector.end());
    
  sput_fail_unless(r1 != r2, "two different seal encryption with same keys can't be the same", __LINE__);
  sput_fail_unless(a1.decrypt(r1) == a2.decrypt(r2.begin(), r2.end()), "decryption", __LINE__);
  try{
    a2.decrypt(r3.begin(), r3.end());
    sput_fail_unless(false, "decryption", __LINE__);
  }catch(themispp::exception& e){} 
    
    
}

void run_secure_cell_test(){
  sput_enter_suite("themispp secure cell seal mode test:");
  sput_run_test(secure_cell_seal_test,"secure_cell_seal_test",__FILE__);
}

int main(){
  sput_start_testing();
  run_secure_cell_test();
  sput_finish_testing();
  return sput_get_return_value();
}
