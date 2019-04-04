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

THEMIS_TEST_SOURCES = $(wildcard tests/themis/*.c)
THEMIS_TEST_HEADERS = $(wildcard tests/themis/*.h)

THEMIS_TEST_SRC = $(THEMIS_TEST_SOURCES)
THEMIS_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(THEMIS_TEST_SRC))

THEMIS_TEST_FMT_SRC = $(THEMIS_TEST_SOURCES) $(THEMIS_TEST_HEADERS)
THEMIS_TEST_FMT_FIXUP = $(patsubst $(TEST_SRC_PATH)/%,$(TEST_OBJ_PATH)/%.fmt_fixup, $(THEMIS_TEST_FMT_SRC))
THEMIS_TEST_FMT_CHECK = $(patsubst $(TEST_SRC_PATH)/%,$(TEST_OBJ_PATH)/%.fmt_check, $(THEMIS_TEST_FMT_SRC))

$(TEST_BIN_PATH)/themis_test: CMD = $(CC) -o $@ $(filter %.o %.a, $^) $(LDFLAGS)

$(TEST_BIN_PATH)/themis_test: $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) $(BIN_PATH)/$(LIBTHEMIS_A) $(BIN_PATH)/$(LIBSOTER_A) $(SOTER_ENGINE_DEPS)
	@echo -n "link "
	@$(BUILD_CMD)
