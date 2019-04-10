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

LIBSOTER_A  = libsoter.a
LIBSOTER_SO = libsoter.$(SHARED_EXT)

SOTER_SOURCES = $(wildcard $(SRC_PATH)/soter/*.c)
SOTER_HEADERS = $(wildcard $(SRC_PATH)/soter/*.h)
ED25519_SOURCES = $(wildcard $(SRC_PATH)/soter/ed25519/*.c)
ED25519_HEADERS = $(wildcard $(SRC_PATH)/soter/ed25519/*.h)

SOTER_SRC = $(SOTER_SOURCES) $(ED25519_SOURCES) $(CRYPTO_ENGINE_SOURCES)

SOTER_AUD_SRC += $(SOTER_SOURCES) $(ED25519_SOURCES) $(CRYPTO_ENGINE_SOURCES)
SOTER_AUD_SRC += $(SOTER_HEADERS) $(ED25519_HEADERS) $(CRYPTO_ENGINE_HEADERS)

# Ignore ed25519 during code reformatting as it is 3rd-party code (and it breaks clang-tidy)
SOTER_FMT_SRC += $(SOTER_SOURCES) $(CRYPTO_ENGINE_SOURCES)
SOTER_FMT_SRC += $(SOTER_HEADERS) $(CRYPTO_ENGINE_HEADERS)

include $(CRYPTO_ENGINE)/soter.mk

SOTER_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(SOTER_SRC))

SOTER_AUD = $(patsubst $(SRC_PATH)/%,$(AUD_PATH)/%, $(SOTER_AUD_SRC))

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(SOTER_FMT_SRC))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(SOTER_FMT_SRC))

SOTER_STATIC = $(BIN_PATH)/$(LIBSOTER_A) $(SOTER_ENGINE_DEPS)

$(BIN_PATH)/$(LIBSOTER_A): CMD = $(AR) rcs $@ $(filter %.o, $^)

$(BIN_PATH)/$(LIBSOTER_A): $(SOTER_OBJ)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

$(BIN_PATH)/$(LIBSOTER_SO): CMD = $(CC) -shared -o $@ $(filter %.o %a, $^) $(LDFLAGS) $(CRYPTO_ENGINE_LDFLAGS)

$(BIN_PATH)/$(LIBSOTER_SO): $(SOTER_OBJ) $(SOTER_ENGINE_DEPS)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
ifdef IS_MACOS
	@install_name_tool -id "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
	@install_name_tool -change "$(BIN_PATH)/$(notdir $@)" "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
endif

$(BIN_PATH)/libsoter.pc:
	@mkdir -p $(BIN_PATH)
	@sed -e "s!%prefix%!$(PREFIX)!" \
	     -e "s!%version%!$(VERSION)!" \
	     -e "s!%crypto-libs%!$(CRYPTO_ENGINE_LDFLAGS)!" \
	    $(SRC_PATH)/soter/libsoter.pc.in > $(BIN_PATH)/libsoter.pc

install_soter: err $(BIN_PATH)/$(LIBSOTER_A) $(BIN_PATH)/$(LIBSOTER_SO) $(BIN_PATH)/libsoter.pc
	@echo -n "install Soter "
	@mkdir -p $(DESTDIR)/$(includedir)/soter
	@mkdir -p $(DESTDIR)/$(pkgconfigdir)
	@mkdir -p $(DESTDIR)/$(libdir)
	@$(INSTALL_DATA) $(SRC_PATH)/soter/*.h              $(DESTDIR)/$(includedir)/soter
	@$(INSTALL_DATA) $(BIN_PATH)/libsoter.pc            $(DESTDIR)/$(pkgconfigdir)
	@$(INSTALL_DATA) $(BIN_PATH)/$(LIBSOTER_A)          $(DESTDIR)/$(libdir)
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(LIBSOTER_SO)      $(DESTDIR)/$(libdir)
	@$(PRINT_OK_)

uninstall_soter:
	@echo -n "uninstall Soter "
	@rm -rf $(DESTDIR)/$(includedir)/soter
	@rm  -f $(DESTDIR)/$(pkgconfigdir)/libsoter.pc
	@rm  -f $(DESTDIR)/$(libdir)/$(LIBSOTER_A)
	@rm  -f $(DESTDIR)/$(libdir)/$(LIBSOTER_SO)
	@$(PRINT_OK_)
