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

CRYPTO_ENGINE_SOURCES += $(wildcard $(CRYPTO_ENGINE)/*.c)
CRYPTO_ENGINE_HEADERS += $(wildcard $(CRYPTO_ENGINE)/*.h)

# Put path to your OpenSSL/LibreSSL here
OPENSSL_DIR = libs/librebin

ifeq ($(CRYPTO_ENGINE_INCLUDE_PATH),)
	CRYPTO_ENGINE_CFLAGS += -Ithird_party/boringssl/src/include
else
	CRYPTO_ENGINE_CFLAGS += -I$(CRYPTO_ENGINE_INCLUDE_PATH)
endif
ifeq ($(CRYPTO_ENGINE_LIB_PATH),)
	CRYPTO_ENGINE_LDFLAGS += -L$(BIN_PATH)/boringssl/crypto -L$(BIN_PATH)/boringssl/decrepit
	SOTER_ENGINE_DEPS += $(BIN_PATH)/boringssl/crypto/libcrypto.a $(BIN_PATH)/boringssl/decrepit/libdecrepit.a
else
	CRYPTO_ENGINE_LDFLAGS += -L$(CRYPTO_ENGINE_LIB_PATH)
endif
CRYPTO_ENGINE_LDFLAGS += -lcrypto -ldecrepit -lpthread

SOTER_ENGINE_CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Release
SOTER_ENGINE_CMAKE_FLAGS += -DCMAKE_C_FLAGS="-fpic"

ifdef IS_EMSCRIPTEN
SOTER_ENGINE_CMAKE_FLAGS += -DOPENSSL_NO_ASM=1
endif

$(BIN_PATH)/boringssl/crypto/libcrypto.a $(BIN_PATH)/boringssl/decrepit/libdecrepit.a:
	@echo "building embedded BoringSSL..."
	@mkdir -p $(BIN_PATH)/boringssl
	@cd $(BIN_PATH)/boringssl && $(CMAKE) $(SOTER_ENGINE_CMAKE_FLAGS) $(abspath third_party/boringssl/src)
	@$(MAKE) -C $(BIN_PATH)/boringssl crypto decrepit
