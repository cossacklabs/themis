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

#include <themispp/secure_cell.hpp>

void info(const char* argv0)
{
    std::cout << "file_encrypter - a sample application for ThemisPP" << std::endl
              << "usage:" << std::endl
              << "    " << argv0 << " {e|d} <password> <input-file> <output-file>" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 5) {
        info(argv[0]);
        return 1;
    }
    std::string command  = argv[1];
    std::string password = argv[2];
    const char* in_path  = argv[3];
    const char* out_path = argv[4];

    // Prepare input file
    std::ifstream in(in_path, std::ifstream::binary);
    if (!in.is_open()) {
        std::cerr << "can't open file " << in_path << std::endl;
        return 1;
    }
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // Query input file length
    in.seekg(0, in.end);
    size_t length = in.tellg();
    in.seekg(0, in.beg);

    // Prepare output file
    std::ofstream out(out_path, std::ofstream::binary);
    if (!out.is_open()) {
        std::cerr << "can't open file " << out_path << std::endl;
        return 1;
    }
    out.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    // Slurp input file
    std::vector<uint8_t> data(length);
    in.read(reinterpret_cast<char*>(&data[0]), length);

    // Initialise Secure Cell in Seal mode
    themispp::secure_cell_seal_with_passphrase scell(password);

    // Encrypt or decrypt data, write out results
    if (command == "e") {
        std::vector<uint8_t> encrypted = scell.encrypt(data);
        out.write(reinterpret_cast<const char*>(&encrypted[0]), encrypted.size());
    } else if (command == "d") {
        std::vector<uint8_t> decrypted = scell.decrypt(data);
        out.write(reinterpret_cast<const char*>(&decrypted[0]), decrypted.size());
    } else {
        info(argv[0]);
        return 1;
    }

    return 0;
}
