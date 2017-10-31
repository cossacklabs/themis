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

include tests/soter/soter.mk
include tests/tools/tools.mk
include tests/themis/themis.mk
include tests/themispp/themispp.mk


nist_rng_test_suite: CMD = $(MAKE) -C $(NIST_STS_DIR)

nist_rng_test_suite:
	@mkdir -p $(NIST_STS_DIR)/obj
	@cd $(NIST_STS_DIR)/experiments && ./create-dir-script
	@$(BUILD_CMD)

soter_test: CMD = $(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS) $(COVERLDFLAGS)

soter_test: nist_rng_test_suite soter_static $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themis_test: CMD = $(CC) -o $(TEST_BIN_PATH)/themis_test $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lthemis -lsoter $(LDFLAGS) $(COVERLDFLAGS)

themis_test: themis_static $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themispp_test: CMD = $(CXX) -o $(TEST_BIN_PATH)/themispp_test $(THEMISPP_TEST_OBJ) -L$(BIN_PATH) -lthemis -lsoter -lstdc++ $(LDFLAGS) $(COVERLDFLAGS)

themispp_test: $(THEMISPP_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

prepare_tests_basic: soter_test themis_test themispp_test

prepare_tests_all: err prepare_tests_basic
ifdef PHP_VERSION
	@echo -n "make tests for phpthemis "
	@echo "#!/bin/bash -e" > ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/scell_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/smessage_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "php -c tests/phpthemis/php.ini ./tests/tools/phpunit.phar ./tests/phpthemis/ssession_test.php" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/phpthemis_test.sh
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
	echo "cd ./tests/jsthemis/" > ./$(BIN_PATH)/tests/node.sh
	echo "wget https://nodejs.org/dist/v4.6.0/node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "tar -xvf node-v4.6.0-linux-x64.tar.gz" >> ./$(BIN_PATH)/tests/node.sh
	echo "cd ../../src/wrappers/themis/jsthemis && PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm pack && mv jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz ../../../../build && cd -" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install mocha" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install nan" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) npm install ../../build/jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz" >> ./$(BIN_PATH)/tests/node.sh
	echo "PATH=`pwd`/tests/jsthemis/node-v4.6.0-linux-x64/bin:$(PATH) ./node_modules/mocha/bin/mocha" >> ./$(BIN_PATH)/tests/node.sh
	chmod a+x ./$(BIN_PATH)/tests/node.sh


test_basic: prepare_tests_basic
	$(TEST_BIN_PATH)/soter_test
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themis_test
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themispp_test
	@echo "------------------------------------------------------------"

# require all dependencies
test: test_basic prepare_tests_all
ifdef PHP_VERSION
	@echo "------------------------------------------------------------"
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/PHP-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/phpthemis_test.sh
	@echo "------------------------------------------------------------"
endif
ifdef PYTHON_VERSION
	@echo "------------------------------------------------------------"
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/Python-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/pythemis_test.sh
	@echo "------------------------------------------------------------"
endif	
ifdef RUBY_GEM_VERSION
	@echo "------------------------------------------------------------"
	@echo "If any error, check https://github.com/cossacklabs/themis/wiki/Ruby-Howto"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/rubythemis_test.sh
	@echo "------------------------------------------------------------"
endif
	$(TEST_BIN_PATH)/node.sh
