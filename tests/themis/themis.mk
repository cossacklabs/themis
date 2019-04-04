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

THEMIS_TEST_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(THEMIS_TEST_SOURCES))

THEMIS_TEST_FMT = $(THEMIS_TEST_SOURCES) $(THEMIS_TEST_HEADERS)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMIS_TEST_FMT))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMIS_TEST_FMT))

$(TEST_BIN_PATH)/themis_test: CMD = $(CC) -o $@ $(filter %.o %.a, $^) $(LDFLAGS) $(CRYPTO_ENGINE_LDFLAGS)

$(TEST_BIN_PATH)/themis_test: $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) $(THEMIS_STATIC)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
