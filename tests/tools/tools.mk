#
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
#

#SPLINT_DIR = tests/tools/splint
#SPLINT_SRC_DIR = $(SPLINT_DIR)/source
#SPLINT_BIN_DIR = $(SPLINT_DIR)/splbin
#SPLINT_EXE = $(SPLINT_BIN_DIR)/bin/splint
#SPLINT_OUT_FILE = static_analysis.txt

#build_splint:
#	mkdir -p $(SPLINT_BIN_DIR)
#	cd $(SPLINT_SRC_DIR) && ./configure --prefix=$(realpath $(SPLINT_BIN_DIR)) && make && make install
#	-rm $(SPLINT_OUT_FILE)

#SPLINT_FILES = $(patsubst %.c,%.splint, $(SOTER_SRC))

#%.splint: %.c
#	-$(SPLINT_EXE) $< -I$(SRC_PATH) -I$(OPENSSL_DIR)/include -D$(CRYPTO_ENGINE_DEF) +posixlib >> $(SPLINT_OUT_FILE)

#static_analysis: build_splint $(SPLINT_FILES)
