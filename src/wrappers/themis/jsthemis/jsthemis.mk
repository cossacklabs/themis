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

JSTHEMIS_SRC = $(SRC_PATH)/wrappers/themis/jsthemis
JSTHEMIS_OBJ = $(OBJ_PATH)/wrappers/themis/jsthemis

JSTHEMIS_SOURCES = $(wildcard $(JSTHEMIS_SRC)/*.cpp)
JSTHEMIS_HEADERS = $(wildcard $(JSTHEMIS_SRC)/*.hpp)

# Unfortunately, clang-tidy requires full compilation flags to be able to work and
# node-gyp add quite a few custom flags with include search paths. It also requires
# some Node modules for compilation. We recreate the same environment here as for
# the "jsthemis_install" target. However, we can't do that without NPM present.
# Therefore we format JsThemis code only if NodeJS is actually installed.
ifdef NPM_VERSION

# A hack to install "nan" before formatting any JsThemis files
FMT_FIXUP += $(JSTHEMIS_OBJ)/node_modules/nan
FMT_CHECK += $(JSTHEMIS_OBJ)/node_modules/nan

$(JSTHEMIS_OBJ)/node_modules/nan:
	@mkdir -p $(JSTHEMIS_OBJ) && cd $(JSTHEMIS_OBJ) && npm install nan && cd -

FMT_FIXUP += $(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%.fmt_fixup,$(JSTHEMIS_SOURCES) $(JSTHEMIS_HEADERS))
FMT_CHECK += $(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%.fmt_check,$(JSTHEMIS_SOURCES) $(JSTHEMIS_HEADERS))

JSTHEMIS_CFLAGS += $(CFLAGS)
JSTHEMIS_CFLAGS += -I$(JSTHEMIS_OBJ)/node_modules/nan

ifdef IS_LINUX
JSTHEMIS_CFLAGS += -I/usr/include/nodejs/src
JSTHEMIS_CFLAGS += -I/usr/include/nodejs/deps/v8/include
endif

$(JSTHEMIS_OBJ)/%.hpp.fmt_fixup $(JSTHEMIS_OBJ)/%.cpp.fmt_fixup: \
	CMD = $(CLANG_TIDY) -fix $< -- $(JSTHEMIS_CFLAGS) 2>/dev/null && $(CLANG_FORMAT) -i $< && touch $@

$(JSTHEMIS_OBJ)/%.hpp.fmt_check $(JSTHEMIS_OBJ)/%.cpp.fmt_check: \
	CMD = $(CLANG_FORMAT) $< | diff -u $< - && $(CLANG_TIDY) $< -- $(JSTHEMIS_CFLAGS) 2>/dev/null && touch $@

endif # ifdef NPM_VERSION
