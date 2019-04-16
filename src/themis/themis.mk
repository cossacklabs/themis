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

LIBTHEMIS_A  = libthemis.a
LIBTHEMIS_SO = libthemis.$(SHARED_EXT)

THEMIS_SOURCES = $(wildcard $(SRC_PATH)/themis/*.c)
THEMIS_HEADERS = $(wildcard $(SRC_PATH)/themis/*.h)

THEMIS_SRC = $(THEMIS_SOURCES)
THEMIS_AUD_SRC = $(THEMIS_SOURCES) $(THEMIS_HEADERS)
THEMIS_FMT_SRC = $(THEMIS_SOURCES) $(THEMIS_HEADERS)

THEMIS_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(THEMIS_SRC))

THEMIS_AUD = $(patsubst $(SRC_PATH)/%,$(AUD_PATH)/%, $(THEMIS_AUD_SRC))

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMIS_FMT_SRC))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMIS_FMT_SRC))

THEMIS_STATIC = $(BIN_PATH)/$(LIBTHEMIS_A) $(SOTER_STATIC)

$(THEMIS_OBJ): CFLAGS += -DTHEMIS_EXPORT

$(BIN_PATH)/$(LIBTHEMIS_A): CMD = $(AR) rcs $@ $(filter %.o, $^)

$(BIN_PATH)/$(LIBTHEMIS_A): $(THEMIS_OBJ)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

$(BIN_PATH)/$(LIBTHEMIS_SO): CMD = $(CC) -shared -o $@ $(filter %.o %.a, $^) $(LDFLAGS) -L$(BIN_PATH) -lsoter

$(BIN_PATH)/$(LIBTHEMIS_SO): $(BIN_PATH)/$(LIBSOTER_SO) $(THEMIS_OBJ)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
ifdef IS_MACOS
	@install_name_tool -id "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
	@install_name_tool -change "$(BIN_PATH)/$(notdir $@)" "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
endif

$(BIN_PATH)/libthemis.pc:
	@mkdir -p $(BIN_PATH)
	@sed -e "s!%prefix%!$(PREFIX)!" \
	     -e "s!%version%!$(VERSION)!" \
	    $(SRC_PATH)/themis/libthemis.pc.in > $(BIN_PATH)/libthemis.pc

install_themis: err $(BIN_PATH)/$(LIBTHEMIS_A) $(BIN_PATH)/$(LIBTHEMIS_SO) $(BIN_PATH)/libthemis.pc
	@echo -n "install Themis "
	@mkdir -p $(DESTDIR)/$(includedir)/themis
	@mkdir -p $(DESTDIR)/$(pkgconfigdir)
	@mkdir -p $(DESTDIR)/$(libdir)
	@$(INSTALL_DATA) $(SRC_PATH)/themis/*.h             $(DESTDIR)/$(includedir)/themis
	@$(INSTALL_DATA) $(BIN_PATH)/libthemis.pc           $(DESTDIR)/$(pkgconfigdir)
	@$(INSTALL_DATA) $(BIN_PATH)/$(LIBTHEMIS_A)         $(DESTDIR)/$(libdir)
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(LIBTHEMIS_SO)     $(DESTDIR)/$(libdir)
	@$(PRINT_OK_)

uninstall_themis:
	@echo -n "uninstall Themis "
	@rm -rf $(DESTDIR)/$(includedir)/themis
	@rm  -f $(DESTDIR)/$(pkgconfigdir)/libthemis.pc
	@rm  -f $(DESTDIR)/$(libdir)/$(LIBTHEMIS_A)
	@rm  -f $(DESTDIR)/$(libdir)/$(LIBTHEMIS_SO)
	@$(PRINT_OK_)
