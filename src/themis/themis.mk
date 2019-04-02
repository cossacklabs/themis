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

THEMIS_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(THEMIS_SRC))

THEMIS_AUD = $(patsubst $(SRC_PATH)/%,$(AUD_PATH)/%, $(THEMIS_AUD_SRC))

THEMIS_FMT_FIXUP = $(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%.fmt_fixup,$(THEMIS_FMT_SRC))
THEMIS_FMT_CHECK = $(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%.fmt_check,$(THEMIS_FMT_SRC))

$(BIN_PATH)/$(LIBTHEMIS_A):  $(THEMIS_OBJ)
$(BIN_PATH)/$(LIBTHEMIS_SO): $(THEMIS_OBJ)

# Link shared Themis library against shared Soter library
$(BIN_PATH)/$(LIBTHEMIS_SO): $(BIN_PATH)/$(LIBSOTER_SO)
$(BIN_PATH)/$(LIBTHEMIS_SO): private LDFLAGS += -L$(BIN_PATH) -lsoter

themis_pkgconfig:
	@mkdir -p $(BIN_PATH)
	@sed -e "s!%prefix%!$(PREFIX)!" \
	     -e "s!%version%!$(VERSION)!" \
	    $(SRC_PATH)/themis/libthemis.pc.in > $(BIN_PATH)/libthemis.pc
