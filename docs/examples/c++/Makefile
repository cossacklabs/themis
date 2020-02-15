# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CXX ?= c++

all: secure_cell_file_encrypter

secure_cell_file_encrypter: secure_cell_file_encrypter.cpp
	$(CXX) -o $@ $< -lthemis -lsoter

all: secure_message_client secure_message_server

secure_message_client: secure_message_client.cpp shared_keys.hpp
	$(CXX) -o $@ $< -lthemis -pthread -lboost_system

secure_message_server: secure_message_server.cpp shared_keys.hpp
	$(CXX) -o $@ $< -lthemis -pthread -lboost_system -lboost_thread

all: secure_session_client secure_session_server

secure_session_client: secure_session_client.cpp shared_keys.hpp
	$(CXX) -o $@ $< -lthemis -pthread -lboost_system

secure_session_server: secure_session_server.cpp shared_keys.hpp
	$(CXX) -o $@ $< -lthemis -pthread -lboost_system -lboost_thread
