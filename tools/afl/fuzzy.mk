#
# Copyright (c) 2019 Cossack Labs Limited
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

AFL_FUZZ ?= afl-fuzz
AFL_CC   ?= afl-clang

# TODO(ilammy, 2021-01-27): use afl++ toolchain if available
# The above tools are compaible with the original "afl" which is currently
# not maintained. Modern systems come with "afl++" that has mostly compatible
# command-line and new instrumentation toolchain. We should use it if possible.

FUZZ_PATH = tools/afl
FUZZ_BIN_PATH = $(BIN_PATH)/afl
FUZZ_SRC_PATH = $(FUZZ_PATH)/src
FUZZ_THEMIS_PATH = $(BIN_PATH)/afl-themis
FUZZ_SOTER_LIB  = $(FUZZ_THEMIS_PATH)/$(LIBSOTER_A)
FUZZ_THEMIS_LIB = $(FUZZ_THEMIS_PATH)/$(LIBTHEMIS_A)

FUZZ_SOURCES = $(wildcard $(FUZZ_SRC_PATH)/*.c)
FUZZ_HEADERS = $(wildcard $(FUZZ_SRC_PATH)/*.h)

FUZZ_TOOLS = $(addprefix $(FUZZ_BIN_PATH)/,$(notdir $(wildcard $(FUZZ_PATH)/input/*)))
FUZZ_OBJS  = $(patsubst $(FUZZ_SRC_PATH)/%.c,$(FUZZ_BIN_PATH)/%.o,$(FUZZ_SOURCES))
FUZZ_UTILS = $(filter-out $(addsuffix .o,$(FUZZ_TOOLS)),$(FUZZ_OBJS))

# Build sources with access to fuzzing headers and link tools to $(FUZZ_THEMIS_LIB).
$(FUZZ_OBJS): CFLAGS += -I$(FUZZ_SRC_PATH)
$(FUZZ_TOOLS): LDFLAGS += $(FUZZ_THEMIS_LIB) $(FUZZ_SOTER_LIB) $(CRYPTO_ENGINE_LDFLAGS)

# afl-clang is partially configured via environment variables. For one, it likes to
# talk on stdout so tell it to pipe down a bit. Additionally, address sanitizer builds
# are usually 32-bit (to keep virtual memory at bay, read AFL docs to learn more).
AFL_CC_ENV += AFL_QUIET=1
ifdef WITH_ASAN
AFL_CC_ENV += AFL_USE_ASAN=1
$(FUZZ_OBJS): CFLAGS += -m32
$(FUZZ_TOOLS): LDFLAGS += -m32
endif
# We do not pass CFLAGS or LDFLAGS to child processes, add the flags again for them
# to be used during recursive make invocation for building Themis.
ifeq ($(AFL_USE_ASAN),1)
CFLAGS  += -m32
LDFLAGS += -m32
endif
# afl-gcc uses AFL_CC environment variable itself. Do not export it to avoid silly
# infinite loops of afl-gcc calling afl-gcc. (afl-clang is fine though.)
unexport AFL_CC

# We don't really track dependencies of $(FUZZ_THEMIS_LIB) here,
# so ask our make to rebuild it every time. The recursively called
# make will figure out what needs to be updated (if anything).
.PHONY: fuzz $(FUZZ_THEMIS_LIB)

# For some reason make treats objects here (and only here) as "intermediate"
# targets and removes them after the build. Ask make to cease and desist.
.PRECIOUS: $(FUZZ_BIN_PATH)/%.o

ifdef FUZZ_BIN
FUZZ_INPUT  := $(FUZZ_PATH)/input/$(FUZZ_BIN)
FUZZ_OUTPUT := $(FUZZ_BIN_PATH)/output/$(FUZZ_BIN)/$(shell date +"%Y-%m-%d_%H-%M-%S")
endif

# american fuzzy lop is expected to be stopped via SIGINT (usually by pressing
# Ctrl-C in terminal). This signal escapes back into make and kills it as well.
# We need to trap the signal to have a chance to tell the user anything.
fuzz: $(FUZZ_TOOLS)
ifdef FUZZ_BIN
	@echo "fuzzing $(FUZZ_BIN)..."
	@mkdir -p $(FUZZ_OUTPUT)
	@trap 'echo "see $(FUZZ_OUTPUT) for results"' SIGINT ; \
	 $(AFL_FUZZ) -i $(FUZZ_INPUT) -o $(FUZZ_OUTPUT) $(FUZZ_BIN_PATH)/$(FUZZ_BIN) @@
endif

$(FUZZ_BIN_PATH)/%.o: $(FUZZ_SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(AFL_CC_ENV) $(AFL_CC) $(CFLAGS) -c -o $@ $<
	@$(PRINT_OK)

$(FUZZ_BIN_PATH)/%: $(FUZZ_BIN_PATH)/%.o $(FUZZ_UTILS) $(FUZZ_THEMIS_LIB)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(AFL_CC_ENV) $(AFL_CC) -o $@ $< $(FUZZ_UTILS) $(LDFLAGS)
	@$(PRINT_OK)

$(FUZZ_THEMIS_LIB): $(SOTER_ENGINE_DEPS)
	@$(AFL_CC_ENV) $(MAKE) themis_static soter_static CC=$(AFL_CC) BUILD_PATH=$(FUZZ_THEMIS_PATH)

FMT_FIXUP += $(patsubst $(FUZZ_SRC_PATH)/%,$(FUZZ_BIN_PATH)/%.fmt_fixup,$(FUZZ_SOURCES) $(FUZZ_HEADERS))
FMT_CHECK += $(patsubst $(FUZZ_SRC_PATH)/%,$(FUZZ_BIN_PATH)/%.fmt_check,$(FUZZ_SOURCES) $(FUZZ_HEADERS))

$(FUZZ_BIN_PATH)/%.c.fmt_fixup $(FUZZ_BIN_PATH)/%.h.fmt_fixup: \
    CMD = $(CLANG_TIDY) -fix $< -- $(CFLAGS) -I$(FUZZ_SRC_PATH) 2>/dev/null && $(CLANG_FORMAT) -i $< && touch $@

$(FUZZ_BIN_PATH)/%.c.fmt_check $(FUZZ_BIN_PATH)/%.h.fmt_check: \
    CMD = $(CLANG_FORMAT) $< | diff -u $< - && $(CLANG_TIDY) $< -- $(CFLAGS) -I$(FUZZ_SRC_PATH) 2>/dev/null && touch $@

$(FUZZ_BIN_PATH)/%.fmt_fixup: $(FUZZ_SRC_PATH)/%
	@mkdir -p $(@D)
	@echo -n "fixup $< "
	@$(BUILD_CMD_)

$(FUZZ_BIN_PATH)/%.fmt_check: $(FUZZ_SRC_PATH)/%
	@mkdir -p $(@D)
	@echo -n "check $< "
	@$(BUILD_CMD_)
