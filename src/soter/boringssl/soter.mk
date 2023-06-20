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

ifneq ($(NINJA),)
SOTER_ENGINE_CMAKE_FLAGS += -G Ninja
endif

# Cross-compilation support for macOS
ifdef IS_MACOS
ifdef SDK
SOTER_ENGINE_CMAKE_FLAGS += -DCMAKE_OSX_SYSROOT=$(SDK)
endif
ifdef ARCH
SOTER_ENGINE_CMAKE_FLAGS += -DCMAKE_OSX_ARCHITECTURES=$(ARCH)
endif
endif

ifdef IS_LINUX
RENAME_BORINGSSL_SYMBOLS = yes
endif
ifdef IS_MACOS
RENAME_BORINGSSL_SYMBOLS = yes
endif
ifdef IS_EMSCRIPTEN
RENAME_BORINGSSL_SYMBOLS = no
SOTER_ENGINE_CMAKE_FLAGS += -DOPENSSL_NO_ASM=1
endif

ifeq ($(RENAME_BORINGSSL_SYMBOLS),yes)
# The prefix must be a valid C identifier. Replace invalid characters.
SOTER_BORINGSSL_PREFIX := $(shell echo "SOTER_$(VERSION)" | tr '.-' '_')

SOTER_CRYPTO_ENGINE_CFLAGS += -DBORINGSSL_PREFIX=$(SOTER_BORINGSSL_PREFIX)
SOTER_CRYPTO_ENGINE_CFLAGS += -I$(BIN_PATH)/boringssl/stage-2/symbol_prefix_include
endif

$(BIN_PATH)/boringssl/crypto/libcrypto.a $(BIN_PATH)/boringssl/decrepit/libdecrepit.a:
	@echo "building embedded BoringSSL..."
	@mkdir -p $(BIN_PATH)/boringssl/stage-1
	@cd $(BIN_PATH)/boringssl/stage-1 && \
	 $(CMAKE) $(SOTER_ENGINE_CMAKE_FLAGS) $(abspath third_party/boringssl/src)
ifeq ($(NINJA),)
	@$(MAKE) -C $(BIN_PATH)/boringssl/stage-1 crypto decrepit
else
	@$(NINJA) -C $(BIN_PATH)/boringssl/stage-1 crypto decrepit
endif
	@mkdir -p $(BIN_PATH)/boringssl/crypto $(BIN_PATH)/boringssl/decrepit
	@cp $(BIN_PATH)/boringssl/stage-1/crypto/libcrypto.a     $(BIN_PATH)/boringssl/crypto/libcrypto.a
	@cp $(BIN_PATH)/boringssl/stage-1/decrepit/libdecrepit.a $(BIN_PATH)/boringssl/decrepit/libdecrepit.a
ifeq ($(RENAME_BORINGSSL_SYMBOLS),yes)
	@cd third_party/boringssl/src && \
	 $(GO) run util/read_symbols.go -out $(abspath $(BIN_PATH)/boringssl/symbols.txt) \
	     $(abspath $(BIN_PATH)/boringssl/stage-1/crypto/libcrypto.a) \
	     $(abspath $(BIN_PATH)/boringssl/stage-1/decrepit/libdecrepit.a)
	@echo "building embedded BoringSSL again with renamed symbols..."
	@mkdir -p $(BIN_PATH)/boringssl/stage-2
	@cd $(BIN_PATH)/boringssl/stage-2 && \
	 $(CMAKE) $(SOTER_ENGINE_CMAKE_FLAGS) \
	     -DBORINGSSL_PREFIX=$(SOTER_BORINGSSL_PREFIX) \
	     -DBORINGSSL_PREFIX_SYMBOLS=$(abspath $(BIN_PATH)/boringssl/symbols.txt) \
	     $(abspath third_party/boringssl/src)
ifeq ($(NINJA),)
	@$(MAKE) -C $(BIN_PATH)/boringssl/stage-2 crypto decrepit
else
	@$(NINJA) -C $(BIN_PATH)/boringssl/stage-2 crypto decrepit
endif
	@mkdir -p $(BIN_PATH)/boringssl/crypto $(BIN_PATH)/boringssl/decrepit
	@cp $(BIN_PATH)/boringssl/stage-2/crypto/libcrypto.a     $(BIN_PATH)/boringssl/crypto/libcrypto.a
	@cp $(BIN_PATH)/boringssl/stage-2/decrepit/libdecrepit.a $(BIN_PATH)/boringssl/decrepit/libdecrepit.a
endif
