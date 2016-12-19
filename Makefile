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

#CC = clang
SRC_PATH = src
ifneq ($(BUILD_PATH),)
	BIN_PATH = $(BUILD_PATH)
else
	BIN_PATH = build
endif
OBJ_PATH = $(BIN_PATH)/obj
TEST_SRC_PATH = tests
TEST_BIN_PATH = $(BIN_PATH)/tests
TEST_OBJ_PATH = $(TEST_BIN_PATH)/obj

CFLAGS += -I$(SRC_PATH) -I$(SRC_PATH)/wrappers/themis/ -I/usr/local/include -fPIC 
LDFLAGS += -L/usr/local/lib

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

AWK_CMD = awk '{ printf "%-30s %-10s\n",$$1, $$2; }'
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_OK_ = printf "$(OK_STRING)\n" | $(AWK_CMD)
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;


define themisecho
      @tput setaf 6
      @echo $1
      @tput sgr0
endef

ifeq ($(ENGINE),)
	ENGINE=libressl
endif

#default engine
ifeq ($(PREFIX),)
PREFIX = /usr
endif

#engine selection block
ifneq ($(ENGINE),)
ifeq ($(ENGINE),openssl)
	CRYPTO_ENGINE_DEF = OPENSSL
	CRYPTO_ENGINE_PATH=openssl
else ifeq ($(ENGINE),libressl)
	CRYPTO_ENGINE_DEF = LIBRESSL	
	CRYPTO_ENGINE_PATH=openssl
else ifeq ($(ENGINE), boringssl)
	CRYPTO_ENGINE_DEF = BORINGSSL
	CRYPTO_ENGINE_PATH=boringssl
else
	ERROR = $(error error: engine $(ENGINE) unsupported...)
endif
endif
#end of engine selection block

CRYPTO_ENGINE = $(SRC_PATH)/soter/$(CRYPTO_ENGINE_PATH)
CFLAGS += -D$(CRYPTO_ENGINE_DEF) -DCRYPTO_ENGINE_PATH=$(CRYPTO_ENGINE_PATH)

ifneq ($(ENGINE_INCLUDE_PATH),)
	CRYPTO_ENGINE_INCLUDE_PATH = $(ENGINE_INCLUDE_PATH)
endif
ifneq ($(ENGINE_LIB_PATH),)
	CRYPTO_ENGINE_LIB_PATH = $(ENGINE_LIB_PATH)
endif
ifneq ($(AUTH_SYM_ALG),)
	CFLAGS += -D$(AUTH_SYM_ALG)
endif
ifneq ($(SYM_ALG),)
	CFLAGS += -D$(SYM_ALG)
endif

ifeq ($(RSA_KEY_LENGTH),1024)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_1024
endif

ifeq ($(RSA_KEY_LENGTH),2048)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_2048
endif

ifeq ($(RSA_KEY_LENGTH),4096)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_4096
endif

ifeq ($(RSA_KEY_LENGTH),8192)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_8192
endif

PHP_VERSION := $(shell php --version 2>/dev/null)
RUBY_GEM_VERSION := $(shell gem --version 2>/dev/null)
PIP_VERSION := $(shell pip --version 2>/dev/null)
PYTHON_VERSION := $(shell python --version 2>&1)
PYTHON3_VERSION := $(shell python3 --version 2>/dev/null)
ifdef PIP_VERSION
PIP_THEMIS_INSTALL := $(shell pip freeze |grep themis)
endif
ifneq ("$(wildcard src/wrappers/themis/php/Makefile)","")
PHP_THEMIS_INSTALL = 1
endif

SHARED_EXT = so

UNAME=$(shell uname)
IS_LINUX = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "linux")
IS_MINGW = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "mingw")
IS_CLANG_COMPILER = $(shell $(CC) --version 2>&1 | $(EGREP) -i -c "clang version")

ifeq ($(shell uname),Darwin)
SHARED_EXT = dylib
PREFIX = /usr/local
ifneq ($(SDK),)
SDK_PLATFORM_VERSION=$(shell xcrun --sdk $(SDK) --show-sdk-platform-version)
XCODE_BASE=$(shell xcode-select --print-path)
CC=$(XCODE_BASE)/usr/bin/gcc
BASE=$(shell xcrun --sdk $(SDK) --show-sdk-platform-path)
SDK_BASE=$(shell xcrun --sdk $(SDK) --show-sdk-path)
FRAMEWORKS=$(SDK_BASE)/System/Library/Frameworks/
SDK_INCLUDES=$(SDK_BASE)/usr/include
CFLAFS += -isysroot $(SDK_BASE) 
endif
ifneq ($(ARCH),)
CFLAFS += -arch $(ARCH)
endif
endif

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

# Should pay attention to warnings (some may be critical for crypto-enabled code (ex. signed-unsigned mismatch)
CFLAGS += -Werror -Wno-switch

ifndef ERROR
include src/soter/soter.mk
include src/themis/themis.mk
include jni/themis_jni.mk
endif


all: err themis_static themis_shared

test_all: err test
ifdef PHP_VERSION
	@echo -n "make tests for phpthemis "
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/scell_test.php" > ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/smessage_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/ssession_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/phpthemis_test.sh
	@$(PRINT_OK_)
endif
ifdef RUBY_GEM_VERSION
	@echo -n "make tests for rubythemis "
	@echo "ruby ./tests/rubythemis/scell_test.rb" > ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/smessage_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/ssession_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/scomparator_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/rubythemis_test.sh
	@$(PRINT_OK_)
endif
ifdef PYTHON_VERSION
	@echo -n "make tests for pythemis "
	@echo "python ./tests/pythemis/scell_test.py" > ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python ./tests/pythemis/smessage_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python ./tests/pythemis/ssession_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python ./tests/pythemis/scomparator_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
ifdef PYTHON3_VERSION
	@echo "echo Python3 $(PYTHON3_VERSION) tests" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "echo ----- pythemis secure cell tests----" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python3 ./tests/pythemis/scell_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "echo ----- pythemis secure message tests----" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python3 ./tests/pythemis/smessage_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "echo ----- pythemis secure session tests----" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python3 ./tests/pythemis/ssession_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python3 ./tests/pythemis/scomparator_test.py" >> ./$(BIN_PATH)/tests/pythemis_test.sh
endif
	@chmod a+x ./$(BIN_PATH)/tests/pythemis_test.sh
	@$(PRINT_OK_)
endif
	echo "cd ./tests/jsthemis/" > ./$(BIN_PATH)/tests/node.sh
	echo "wget https://nodejs.org/dist/v4.6.0/node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "tar -xvf node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "cd ../../src/wrappers/themis/jsthemis && PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm pack && mv jsthemis-0.9.4.tgz ../../../../build && cd -" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install mocha" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install nan" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install ../../build/jsthemis-0.9.4.tgz" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) ./node_modules/mocha/bin/mocha" >> ./$(BIN_PATH)/tests/node.sh
	chmod a+x ./$(BIN_PATH)/tests/node.sh

soter_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)

soter_static: $(SOTER_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

soter_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT) $(SOTER_OBJ) $(LDFLAGS)

soter_shared: $(SOTER_OBJ)
	@echo -n "link "
ifeq ($(shell uname),Darwin)
	@install_name_tool -id "$(PREFIX)/lib/lib$(SOTER_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)
	@install_name_tool -change "$(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)" "$(PREFIX)/lib/lib(SOTER_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)
endif
	@$(BUILD_CMD)

themis_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(THEMIS_BIN).a $(THEMIS_OBJ)

themis_static: soter_static $(THEMIS_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themis_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT) $(THEMIS_OBJ) -L$(BIN_PATH) -l$(SOTER_BIN)

themis_shared: soter_shared $(THEMIS_OBJ)
	@echo -n "link "
ifeq ($(shell uname),Darwin)
	@install_name_tool -id "$(PREFIX)/lib/lib$(THEMIS_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)
	@install_name_tool -change "$(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)" "$(PREFIX)/lib/lib$(THEMIS_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)
endif
	@$(BUILD_CMD)

themis_jni: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_JNI_BIN).$(SHARED_EXT) $(THEMIS_JNI_OBJ) -L$(BIN_PATH) -l$(THEMIS_BIN) -l$(SOTER_BIN)

themis_jni: themis_static $(THEMIS_JNI_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


$(OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -DNIST_STS_EXE_PATH=$(realpath $(NIST_STS_DIR)) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.opp: CMD = $(CXX) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.opp: $(TEST_SRC_PATH)/%.cpp
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

include tests/test.mk

err: ; $(ERROR)

clean: CMD = rm -rf $(BIN_PATH)

clean: nist_rng_test_suite
	@$(BUILD_CMD)

make_install_dirs: CMD = mkdir -p $(PREFIX)/include/themis $(PREFIX)/include/soter $(PREFIX)/lib

make_install_dirs:
	@echo -n "making dirs for install "
	@$(BUILD_CMD_)

install_soter_headers: CMD = install $(SRC_PATH)/soter/*.h $(PREFIX)/include/soter

install_soter_headers: err all make_install_dirs
	@echo -n "install soter headers "
	@$(BUILD_CMD_)

install_themis_headers: CMD = install $(SRC_PATH)/themis/*.h $(PREFIX)/include/themis

install_themis_headers: err all make_install_dirs
	@echo -n "install themis headers "
	@$(BUILD_CMD_)

install_static_libs: CMD = install $(BIN_PATH)/*.a $(PREFIX)/lib

install_static_libs: err all make_install_dirs
	@echo -n "install static libraries "
	@$(BUILD_CMD_)

install_shared_libs: CMD = install $(BIN_PATH)/*.$(SHARED_EXT) $(PREFIX)/lib

install_shared_libs: err all make_install_dirs
	@echo -n "install shared libraries "
	@$(BUILD_CMD_)

install: install_soter_headers install_themis_headers install_static_libs install_shared_libs

phpthemis_uninstall: CMD = if [ -e src/wrappers/themis/php/Makefile ]; then cd src/wrappers/themis/php && make distclean ; fi;

phpthemis_uninstall:
ifdef PHP_THEMIS_INSTALL
	@echo -n "phpthemis uninstall "
	@$(BUILD_CMD_)
endif

rubythemis_uninstall: CMD = gem uninstall themis

rubythemis_uninstall:
ifdef RUBY_GEM_VERSION
	@echo -n "rubythemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: CMD = rm -rf $(PREFIX)/include/themis && rm -rf $(PREFIX)/include/soter && rm -f $(PREFIX)/lib/libsoter.a && rm -f $(PREFIX)/lib/libthemis.a && rm -f $(PREFIX)/lib/libsoter.so && rm -f $(PREFIX)/lib/libthemis.so && rm -f $(PREFIX)/lib/libsoter.dylib && rm -f $(PREFIX)/lib/libthemis.dylib

uninstall: phpthemis_uninstall rubythemis_uninstall themispp_uninstall
	@echo -n "themis uninstall "
	@$(BUILD_CMD_)

phpthemis_install: CMD = cd src/wrappers/themis/php && phpize && ./configure && make install

phpthemis_install: install
ifdef PHP_VERSION
	@echo -n "phpthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: php not found"
	@exit 1
endif

rubythemis_install: CMD = cd src/wrappers/themis/ruby && gem build rubythemis.gemspec && gem install ./*.gem

rubythemis_install: install
ifdef RUBY_GEM_VERSION
	@echo -n "rubythemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: ruby gem not found"
	@exit 1
endif

pythemis_install: CMD = cd src/wrappers/themis/python/ && python2 setup.py install --record files.txt

pythemis_install: install
ifdef PYTHON_VERSION
	@echo -n "pythemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: python not found"
	@exit 1
endif
ifdef PYTHON3_VERSION
	@cd src/wrappers/themis/python/ && python3 setup.py install --record files3.txt
endif

themispp_install: CMD = install $(SRC_PATH)/wrappers/themis/themispp/*.hpp $(PREFIX)/include/themispp

themispp_install: install
	@mkdir -p $(PREFIX)/include/themispp
	@$(BUILD_CMD)

themispp_uninstall: CMD = rm -rf $(PREFIX)/include/themispp

themispp_uninstall: 
	@echo -n "themispp uninstall "
	@$(BUILD_CMD_)

