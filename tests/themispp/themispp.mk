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

THEMISPP_TEST_SOURCES = $(wildcard $(TEST_SRC_PATH)/themispp/*.cpp)
THEMISPP_TEST_HEADERS = $(wildcard $(TEST_SRC_PATH)/themispp/*.hpp)

THEMISPP_TEST_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(THEMISPP_TEST_SOURCES))

THEMISPP_TEST_FMT = $(THEMISPP_TEST_SOURCES) $(THEMISPP_TEST_HEADERS)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMISPP_TEST_FMT))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMISPP_TEST_FMT))

ifdef IS_EMSCRIPTEN
# Emscripten does not support dynamic linkage, do a static build for it.
THEMISPP_TEST_LDFLAGS += -s SINGLE_FILE=1
$(TEST_BIN_PATH)/themispp_test: $(THEMIS_STATIC)
else
# Link dynamically against Themis library in the build directory,
# not the one in the standard system paths (if any).
# We also need to link against Soter explicitly because of private imports.
THEMISPP_TEST_LDFLAGS += -L$(BIN_PATH) -lthemis -lsoter
ifdef IS_LINUX
THEMISPP_TEST_LDFLAGS += -Wl,-rpath,$(abspath $(BIN_PATH))
endif
$(TEST_BIN_PATH)/themispp_test: $(BIN_PATH)/$(LIBTHEMIS_SO)
endif

$(TEST_BIN_PATH)/themispp_test: CMD = $(CXX) -o $@ $(filter %.o %.a, $^) $(LDFLAGS) $(THEMISPP_TEST_LDFLAGS)

$(TEST_BIN_PATH)/themispp_test: $(THEMISPP_TEST_OBJ) $(COMMON_TEST_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

clean_themispp_test:
	@rm -f $(THEMISPP_TEST_OBJ)
