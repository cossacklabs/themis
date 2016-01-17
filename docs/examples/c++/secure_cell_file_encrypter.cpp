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
#include <fstream>
#include <string.h>
#include <themispp/secure_cell.hpp>

void info(){
  std::cout<<"file_encrypter - an sample application for ThemisPP"<<std::endl
	   <<"usage file_encrypter [de] <password> <in_file_name> <out_file_name>"<<std::endl;
}

int main(int argc, char* argv[]){
  if(argc!=5){
    info();
    return 1;
  }
  std::ifstream in(argv[3], std::ifstream::binary);
  if(!in.is_open()){
    std::cerr<<"can`t open file "<<argv[3]<<std::endl;
    return 1;
  }
  in.seekg (0, in.end);
  int length = in.tellg();
  in.seekg (0, in.beg);
  
  std::ofstream out(argv[4], std::ifstream::binary);
  if(!out.is_open()){
    std::cerr<<"can`t open file "<<argv[3]<<std::endl;
    return 1;
  }

  std::vector<uint8_t> data(length);
  in.read((char*)(&data[0]), length);
  themispp::secure_cell_seal_t sc(std::vector<uint8_t>(argv[2], argv[2]+strlen(argv[2])));
  if(std::string(argv[1]) == "d"){
    //decrypt_file;
    try{
      const std::vector<uint8_t>& d(sc.decrypt(data));
      out.write(reinterpret_cast<const char*>(&d[0]), d.size());
    }catch(themispp::exception_t& e){
      std::cerr<<"decryption error: "<<e.what()<<std::endl;
    }
  } else if (std::string(argv[1]) == "e"){
    //encrypt_file
    try{
      const std::vector<uint8_t>& d(sc.encrypt(data));
      out.write(reinterpret_cast<const char*>(&d[0]), d.size());
    }catch(themispp::exception_t& e){
      std::cerr<<"decryption error: "<<e.what()<<std::endl;
    }
  } else {
    info();
  }
  in.close();
  out.close();
  return 0;
}
