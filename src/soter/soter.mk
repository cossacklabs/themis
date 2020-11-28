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

LIBSOTER_A = libsoter.a
LIBSOTER_SO = libsoter.$(SHARED_EXT)
LIBSOTER_LINK = libsoter.$(SHARED_EXT)

ifdef IS_LINUX
LIBSOTER_SO = libsoter.$(SHARED_EXT).$(LIBRARY_SO_VERSION)
LIBSOTER_SO_LDFLAGS = -Wl,-soname,$(LIBSOTER_SO)
endif
ifdef IS_MACOS
LIBSOTER_SO = libsoter.$(LIBRARY_SO_VERSION).$(SHARED_EXT)
endif
ifdef IS_MSYS
LIBSOTER_SO = msys-soter-$(LIBRARY_SO_VERSION).$(SHARED_EXT)
LIBSOTER_LINK =
LIBSOTER_IMPORT = libsoter.dll.a
LIBSOTER_SO_LDFLAGS = -Wl,-out-implib,$(BIN_PATH)/$(LIBSOTER_IMPORT)
endif

SOTER_SOURCES = $(wildcard $(SRC_PATH)/soter/*.c)
SOTER_HEADERS += $(wildcard $(INC_PATH)/soter/*.h)
SOTER_HEADERS += $(wildcard $(SRC_PATH)/soter/*.h)
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

SOTER_STATIC = $(BIN_PATH)/$(LIBSOTER_A)

$(SOTER_OBJ): CFLAGS += -DSOTER_EXPORT
$(SOTER_OBJ): CFLAGS += $(SOTER_CRYPTO_ENGINE_CFLAGS)

# First build Soter library, then merge embedded crypto engine libs into it.
# On macOS this may cause warnings about files with no symbols in BoringSSL,
# suppress those warnings with some Bash wizardry.
$(BIN_PATH)/$(LIBSOTER_A): CMD = $(AR) rcs $@ $(filter %.o, $^) \
    && scripts/merge-static-libs.sh $@ $(filter %.a, $^) \
    $(if $(IS_MACOS),> >(grep -v 'has no symbols$$'))

# Make sure to build dependencies before objects. This is important in case
# of embedded BoringSSL with renamed symbols: they need to be renamed before
# Soter's objects are built against them.
$(SOTER_OBJ): $(SOTER_ENGINE_DEPS)

$(BIN_PATH)/$(LIBSOTER_A): $(SOTER_OBJ) $(SOTER_ENGINE_DEPS)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

$(BIN_PATH)/$(LIBSOTER_SO): CMD = $(CC) -shared -o $@ $(filter %.o %a, $^) $(LDFLAGS) $(CRYPTO_ENGINE_LDFLAGS) $(LIBSOTER_SO_LDFLAGS)

$(BIN_PATH)/$(LIBSOTER_SO): $(SOTER_OBJ) $(SOTER_ENGINE_DEPS)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
ifneq ($(LIBSOTER_SO),$(LIBSOTER_LINK))
	@ln -sf $(LIBSOTER_SO) $(BIN_PATH)/$(LIBSOTER_LINK)
endif

$(BIN_PATH)/libsoter.pc:
	@mkdir -p $(BIN_PATH)
	@sed -e "s!%libdir%!$(libdir)!" \
	     -e "s!%includedir%!$(includedir)!" \
	     -e "s!%version%!$(VERSION)!" \
	     -e "s!%crypto-libs%!$(CRYPTO_ENGINE_LDFLAGS)!" \
	    $(SRC_PATH)/soter/libsoter.pc.in > $(BIN_PATH)/libsoter.pc

install_soter: $(BIN_PATH)/$(LIBSOTER_A) $(BIN_PATH)/$(LIBSOTER_SO) $(BIN_PATH)/libsoter.pc
	@echo -n "install Soter "
	@mkdir -p $(DESTDIR)$(includedir)/soter
	@mkdir -p $(DESTDIR)$(pkgconfigdir)
ifdef IS_MSYS
	@mkdir -p $(DESTDIR)$(bindir)
endif
	@mkdir -p $(DESTDIR)$(libdir)
	@$(INSTALL_DATA) $(INC_PATH)/soter/*.h              $(DESTDIR)$(includedir)/soter
	@$(INSTALL_DATA) $(BIN_PATH)/libsoter.pc            $(DESTDIR)$(pkgconfigdir)
	@$(INSTALL_DATA) $(BIN_PATH)/$(LIBSOTER_A)          $(DESTDIR)$(libdir)
ifdef IS_MSYS
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(LIBSOTER_SO)      $(DESTDIR)$(bindir)
else
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(LIBSOTER_SO)      $(DESTDIR)$(libdir)
endif
ifdef IS_MACOS
	@install_name_tool -id "$(libdir)/$(LIBSOTER_SO)" "$(DESTDIR)$(libdir)/$(LIBSOTER_SO)"
	@install_name_tool -change "$(BIN_PATH)/$(LIBSOTER_SO)" "$(libdir)/$(LIBSOTER_SO)" "$(DESTDIR)$(libdir)/$(LIBSOTER_SO)"
endif
ifneq ($(LIBSOTER_IMPORT),)
	@$(INSTALL_DATA) $(BIN_PATH)/$(LIBSOTER_IMPORT)     $(DESTDIR)$(libdir)
endif
ifneq ($(LIBSOTER_LINK),)
	@ln -sf $(LIBSOTER_SO)                              $(DESTDIR)$(libdir)/$(LIBSOTER_LINK)
endif
	@$(PRINT_OK_)

uninstall_soter:
	@echo -n "uninstall Soter "
	@rm -rf $(DESTDIR)$(includedir)/soter
	@rm  -f $(DESTDIR)$(pkgconfigdir)/libsoter.pc
	@rm  -f $(DESTDIR)$(libdir)/$(LIBSOTER_A)
ifdef IS_MSYS
	@rm  -f $(DESTDIR)$(bindir)/$(LIBSOTER_SO)
	@rm  -f $(DESTDIR)$(libdir)/$(LIBSOTER_IMPORT)
else
	@rm  -f $(DESTDIR)$(libdir)/$(LIBSOTER_SO)
	@rm  -f $(DESTDIR)$(libdir)/$(LIBSOTER_LINK)
endif
	@$(PRINT_OK_)
