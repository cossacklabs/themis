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
THEMIS_TEST_BIN = $(TEST_BIN_PATH)/themis_test.js
else
THEMIS_TEST_BIN = $(TEST_BIN_PATH)/themis_test
endif

THEMIS_TEST_SOURCES = $(wildcard tests/themis/*.c)
THEMIS_TEST_HEADERS = $(wildcard tests/themis/*.h)

THEMIS_TEST_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(THEMIS_TEST_SOURCES))

THEMIS_TEST_FMT = $(THEMIS_TEST_SOURCES) $(THEMIS_TEST_HEADERS)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMIS_TEST_FMT))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMIS_TEST_FMT))

ifdef IS_EMSCRIPTEN
THEMIS_TEST_LDFLAGS += -s SINGLE_FILE=1
endif

# Link dynamically against the Themis library in the build directory,
# not the one in the standard system paths (if any).
# We also need to link against Soter explicitly because of private imports.
THEMIS_TEST_LDFLAGS += -L$(BIN_PATH) -lthemis -lsoter
ifdef IS_LINUX
THEMIS_TEST_LDFLAGS += -Wl,-rpath,$(abspath $(BIN_PATH))
endif

$(THEMIS_TEST_BIN): CMD = $(CC) -o $@ $(filter %.o %.a, $^) $(LDFLAGS) $(THEMIS_TEST_LDFLAGS)

$(THEMIS_TEST_BIN): $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) $(BIN_PATH)/$(LIBTHEMIS_SO)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
