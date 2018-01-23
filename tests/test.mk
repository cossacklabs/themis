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

COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

NIST_STS_DIR = tests/soter/nist-sts

GOTHEMIS_IMPORT = github.com/cossacklabs/themis/gothemis

include tests/soter/soter.mk
include tests/tools/tools.mk
include tests/themis/themis.mk
include tests/themispp/themispp.mk


nist_rng_test_suite: CMD = $(MAKE) -C $(NIST_STS_DIR)

nist_rng_test_suite:
	@mkdir -p $(NIST_STS_DIR)/obj
	@cd $(NIST_STS_DIR)/experiments && ./create-dir-script
	@$(BUILD_CMD)

nist_rng_test_suite_clean: 
	@echo "cleaning nist suit"
	@make clean -C $(NIST_STS_DIR)

soter_test: CMD = $(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS) $(COVERLDFLAGS)

soter_test: nist_rng_test_suite soter_static $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themis_test: CMD = $(CC) -o $(TEST_BIN_PATH)/themis_test $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) $(CFLAGS) -L$(BIN_PATH) -lthemis -lsoter $(LDFLAGS) $(COVERLDFLAGS)

themis_test: themis_static $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themispp_test: CMD = $(CXX) -o $(TEST_BIN_PATH)/themispp_test $(THEMISPP_TEST_OBJ) -L$(BIN_PATH) -lthemis -lsoter -lstdc++ $(LDFLAGS) $(COVERLDFLAGS)

themispp_test: $(THEMISPP_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

prepare_tests_basic: soter_test themis_test

prepare_tests_all: err prepare_tests_basic themispp_test
ifdef PHP_VERSION
	@echo -n "make tests for phpthemis "
	@echo "#!/bin/bash -e" > ./$(BIN_PATH)/tests/phpthemis_test.sh
	# @echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/scell_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	# @echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/smessage_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	# @echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/ssession_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	# @cp ./src/wrappers/themis/$(PHP_FOLDER)/.libs/phpthemis.so ./tests/phpthemis/phpthemis.so
	@echo "cd tests/phpthemis; bash ./run_tests.sh" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/phpthemis_test.sh
	@cd ./tests/phpthemis; ln -s ../../src/wrappers/themis/$(PHP_FOLDER)/.libs/phpthemis.so ./phpthemis.so
	@$(PRINT_OK_)
endif
ifdef RUBY_GEM_VERSION
	@echo -n "make tests for rubythemis "
	@echo "#!/bin/bash -e" > ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/scell_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/smessage_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/ssession_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@echo "ruby ./tests/rubythemis/scomparator_test.rb" >> ./$(BIN_PATH)/tests/rubythemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/rubythemis_test.sh
	@$(PRINT_OK_)
endif
ifdef PYTHON_VERSION
	@echo -n "make tests for pythemis "
	@echo "#!/bin/bash -e" > ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python -m unittest discover -s tests/pythemis" >> ./$(BIN_PATH)/tests/pythemis_test.sh
ifdef PYTHON3_VERSION
	@echo "echo Python3 $(PYTHON3_VERSION) tests" >> ./$(BIN_PATH)/tests/pythemis_test.sh
	@echo "python3 -m unittest discover -s tests/pythemis" >> ./$(BIN_PATH)/tests/pythemis_test.sh
endif
	@chmod a+x ./$(BIN_PATH)/tests/pythemis_test.sh
	@$(PRINT_OK_)
endif
ifdef NPM_VERSION
	@echo -n "make tests for jsthemis "
	echo "cd ./tests/jsthemis/" > ./$(BIN_PATH)/tests/node.sh
	echo "wget https://nodejs.org/dist/v4.6.0/node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "tar -xvf node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "cd ../../src/wrappers/themis/jsthemis && PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm pack && mv jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz ../../../../build && cd -" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install mocha" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install nan" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install ../../build/jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) ./node_modules/mocha/bin/mocha" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) rm -rf ./node_modules" >> ./$(BIN_PATH)/tests/node.sh
	chmod a+x ./$(BIN_PATH)/tests/node.sh
	@$(PRINT_OK_)
endif



test: prepare_tests_basic
	@echo "------------------------------------------------------------"
	@echo "Running themis-core basic tests."
	$(TEST_BIN_PATH)/soter_test
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themis_test
	@echo "------------------------------------------------------------"

# require all dependencies to be installed
test_cpp:
	@echo "------------------------------------------------------------"
	@echo "Running themissp tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/CPP-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themispp_test
	@echo "------------------------------------------------------------"

test_php:
ifdef PHP_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running phpthemis tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/PHP-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/phpthemis_test.sh
	@echo "------------------------------------------------------------"
endif

test_python:
ifdef PYTHON_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running pythemis tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/Python-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/pythemis_test.sh
	@echo "------------------------------------------------------------"
endif

test_ruby:
ifdef RUBY_GEM_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running rubythemis tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/Ruby-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/rubythemis_test.sh
	@echo "------------------------------------------------------------"
endif

test_js:
ifdef NPM_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running jsthemis tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/NodeJS-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/node.sh
endif

test_go:
ifdef GO_VERSION	
	@echo "------------------------------------------------------------"
	@echo "Running gothemis tests."
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/Go-HowTo"
	@echo "------------------------------------------------------------"
	@go test -v $(GOTHEMIS_IMPORT)/...
endif

test_all: test prepare_tests_all test_cpp test_php test_python test_ruby test_js test_go
