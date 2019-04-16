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

ifdef IS_EMSCRIPTEN
SOTER_TEST_BIN = $(TEST_BIN_PATH)/soter_test.js
else
SOTER_TEST_BIN = $(TEST_BIN_PATH)/soter_test
endif

NIST_STS_DIR = tests/soter/nist-sts

SOTER_TEST_SOURCES = $(wildcard tests/soter/*.c)
SOTER_TEST_HEADERS = $(wildcard tests/soter/*.h)

SOTER_TEST_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(SOTER_TEST_SOURCES))

SOTER_TEST_FMT = $(SOTER_TEST_SOURCES) $(SOTER_TEST_HEADERS)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(SOTER_TEST_FMT))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(SOTER_TEST_FMT))

ifdef IS_EMSCRIPTEN
$(SOTER_TEST_BIN): LDFLAGS += -s SINGLE_FILE=1

# Emscripten cannot conveniently run NIST STS therefore we disable it.
NO_NIST_STS := true
endif

ifdef NO_NIST_STS
$(OBJ_PATH)/tests/soter/%: CFLAGS += -DNO_NIST_STS=1
else
$(OBJ_PATH)/tests/soter/%: CFLAGS += -DNIST_STS_EXE_PATH=$(realpath $(NIST_STS_DIR))

$(SOTER_TEST_BIN): nist_rng_test_suite
endif

$(SOTER_TEST_BIN): CMD = $(CC) -o $@ $(filter %.o %.a, $^) $(LDFLAGS) $(CRYPTO_ENGINE_LDFLAGS)

$(SOTER_TEST_BIN): $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) $(SOTER_STATIC)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

nist_rng_test_suite:
	@mkdir -p $(NIST_STS_DIR)/obj
	@cd $(NIST_STS_DIR)/experiments && ./create-dir-script
	@$(MAKE) --quiet -C $(NIST_STS_DIR)

nist_rng_test_suite_clean:
	@$(MAKE) --quiet -C $(NIST_STS_DIR) clean
