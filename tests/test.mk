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
COMMON_TEST_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

include tests/soter/soter.mk
include tests/tools/tools.mk
include tests/themis/themis.mk
include tests/themispp/themispp.mk
include tests/themispp_simple/themispp_simple.mk

soter_test:    $(SOTER_TEST_BIN)
themis_test:   $(THEMIS_TEST_BIN)
themispp_test: $(TEST_BIN_PATH)/themispp_test

$(OBJ_PATH)/tests/%: CFLAGS += -I$(TEST_SRC_PATH)

GOTHEMIS_SRC = gothemis

PYTHON2_TEST_SCRIPT=$(BIN_PATH)/tests/pythemis2_test.sh
PYTHON3_TEST_SCRIPT=$(BIN_PATH)/tests/pythemis3_test.sh

rustthemis_integration_tools:
	@echo "make integration tools for rust-themis..."
	@cargo build --package themis-integration-tools
	@for tool in $(notdir $(foreach tool,$(wildcard tools/rust/*.rs),$(basename $(tool)))); \
	do cp target/debug/$$tool tools/rust/$$tool.rust; done
	@$(PRINT_OK_)

gothemis_integration_tools:
	@echo "make integration tools for GoThemis..."
	@cd tools/go && for tool in *.go; do go build -o "$$tool.compiled" "$$tool"; done
	@$(PRINT_OK_)

prepare_tests_basic: soter_test themis_test

prepare_tests_all: prepare_tests_basic themispp_test
ifdef PHP_VERSION
	@echo -n "make tests for phpthemis "
	@echo "#!/usr/bin/env bash" > ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "set -eu" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@echo "cd tests/phpthemis; bash ./run_tests.sh" >> ./$(BIN_PATH)/tests/phpthemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/phpthemis_test.sh
	@$(PRINT_OK_)
endif
ifdef RUBY_GEM_VERSION
	@echo -n "make tests for rbthemis "
	@echo "#!/usr/bin/env bash" > ./$(BIN_PATH)/tests/rbthemis_test.sh
	@echo "set -eu" >> ./$(BIN_PATH)/tests/rbthemis_test.sh
	@echo "ruby ./tests/rbthemis/scell_test.rb" >> ./$(BIN_PATH)/tests/rbthemis_test.sh
	@echo "ruby ./tests/rbthemis/smessage_test.rb" >> ./$(BIN_PATH)/tests/rbthemis_test.sh
	@echo "ruby ./tests/rbthemis/ssession_test.rb" >> ./$(BIN_PATH)/tests/rbthemis_test.sh
	@echo "ruby ./tests/rbthemis/scomparator_test.rb" >> ./$(BIN_PATH)/tests/rbthemis_test.sh
	@chmod a+x ./$(BIN_PATH)/tests/rbthemis_test.sh
	@$(PRINT_OK_)
endif
ifdef PYTHON3_VERSION
	@echo -n "make tests for pythemis with python3 "
	@echo "#!/usr/bin/env bash" > ./$(PYTHON3_TEST_SCRIPT)
	@echo "set -eu" >> ./$(PYTHON3_TEST_SCRIPT)
	@echo "python3 -m unittest discover -s tests/pythemis" >> ./$(PYTHON3_TEST_SCRIPT)
	@chmod a+x ./$(PYTHON3_TEST_SCRIPT)
	@$(PRINT_OK_)
endif

ifdef IS_EMSCRIPTEN
RUN_TEST = node
endif

test: prepare_tests_basic
ifdef IS_EMSCRIPTEN
ifeq ($(NODE_VERSION),)
	@echo 2>&1 "------------------------------------------------------------"
	@echo 2>&1 "Node.js is not installed. Cannot run tests in Emscripten environment."
	@echo 2>&1 ""
	@echo 2>&1 "Make sure you have \"node\" binary available in PATH and try again."
	@echo 2>&1 "------------------------------------------------------------"
	@exit 1
endif
endif
	@echo "------------------------------------------------------------"
	@echo "Running themis-core basic tests."
	$(RUN_TEST) $(SOTER_TEST_BIN)
	@echo "------------------------------------------------------------"
	$(RUN_TEST) $(THEMIS_TEST_BIN)
	@echo "------------------------------------------------------------"

# require all dependencies to be installed
test_cpp:
	@echo "------------------------------------------------------------"
	@echo "Running themissp tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/cpp/"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themispp_test
	@echo "------------------------------------------------------------"

test_php:
ifdef PHP_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running phpthemis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/php/"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/phpthemis_test.sh
	@echo "------------------------------------------------------------"
endif

test_python:
# run test if any of python version available
ifneq ($(PYTHON3_VERSION),)
	@echo "------------------------------------------------------------"
	@echo "Running pythemis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/python/"
	@echo "------------------------------------------------------------"
	$(PYTHON3_TEST_SCRIPT)
	@echo "------------------------------------------------------------"
else
	@echo "python3 not found"
	@exit 1
endif

test_ruby:
ifdef RUBY_GEM_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running rbthemis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/ruby/"
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/rbthemis_test.sh
	@echo "------------------------------------------------------------"
endif

test_js:
ifdef NPM_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running jsthemis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/nodejs/"
	@echo "------------------------------------------------------------"
	cd $(JSTHEMIS_SRC) && npm run install_centos7_specific_deps && npm install && npm test
	@echo "------------------------------------------------------------"
endif

test_go:
ifdef GO_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running gothemis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/go/"
	@echo "------------------------------------------------------------"
	@cd $(GOTHEMIS_SRC) && go test -v ./...
endif

test_rust:
ifdef RUST_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running rust-themis tests."
	@echo "In case of errors, see https://docs.cossacklabs.com/themis/languages/rust/"
	@echo "------------------------------------------------------------"
	$(TEST_SRC_PATH)/rust/run_tests.sh
	@echo "------------------------------------------------------------"
endif

test_wasm:
ifdef NPM_VERSION
	@echo "------------------------------------------------------------"
	@echo "Running wasm-themis tests."
	@echo "------------------------------------------------------------"
	cd $(WASM_PATH) && npm install && npm test
	@echo "------------------------------------------------------------"
endif

test_all: test prepare_tests_all test_cpp test_php test_python test_ruby test_js test_go test_rust

# requires all dependencies to be installed in system paths
test_cpp_simple: $(TEST_BIN_PATH)/themispp_simple_test
	@echo "------------------------------------------------------------"
	@echo "Running themissp simple test."
	@echo "------------------------------------------------------------"
	$(TEST_BIN_PATH)/themispp_simple_test
	@echo "------------------------------------------------------------"
