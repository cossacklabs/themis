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
JSTHEMIS_OBJ = $(OBJ_PATH)/$(JSTHEMIS_SRC)

JSTHEMIS_SOURCES = $(wildcard $(JSTHEMIS_SRC)/*.cpp)
JSTHEMIS_HEADERS = $(wildcard $(JSTHEMIS_SRC)/*.hpp)
JSTHEMIS_PACKAGE += $(JSTHEMIS_SRC)/README.md
JSTHEMIS_PACKAGE += $(JSTHEMIS_SRC)/LICENSE
JSTHEMIS_PACKAGE += $(JSTHEMIS_SRC)/package.json

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

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(JSTHEMIS_SOURCES) $(JSTHEMIS_HEADERS))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(JSTHEMIS_SOURCES) $(JSTHEMIS_HEADERS))

$(JSTHEMIS_OBJ)/%: CFLAGS += $(JSTHEMIS_CFLAGS)

JSTHEMIS_CFLAGS += -I$(JSTHEMIS_OBJ)/node_modules/nan

ifdef IS_LINUX
JSTHEMIS_CFLAGS += -I/usr/include/nodejs/src
JSTHEMIS_CFLAGS += -I/usr/include/nodejs/deps/v8/include
endif

else # ifdef NPM_VERSION

FMT_FIXUP += $(JSTHEMIS_OBJ)/warning_fixup
FMT_CHECK += $(JSTHEMIS_OBJ)/warning_check

$(JSTHEMIS_OBJ)/warning_fixup:
	$(warning NodeJS not installed, JsThemis code will not be formatted)

$(JSTHEMIS_OBJ)/warning_check:
	$(warning NodeJS not installed, JsThemis code will not be checked)

endif # ifdef NPM_VERSION

$(BUILD_PATH)/jsthemis.tgz: $(JSTHEMIS_SOURCES) $(JSTHEMIS_HEADERS) $(JSTHEMIS_PACKAGE)
	@cd $(BUILD_PATH) && npm pack $(abspath $(JSTHEMIS_SRC)) > /dev/null
	@mv $(BUILD_PATH)/jsthemis-*.tgz $(BUILD_PATH)/jsthemis.tgz
	@echo $(BUILD_PATH)/jsthemis.tgz

jsthemis: $(BUILD_PATH)/jsthemis.tgz

jsthemis_install: CMD = npm install $(abspath $(BUILD_PATH)/jsthemis.tgz)
jsthemis_install: $(BUILD_PATH)/jsthemis.tgz
ifdef NPM_VERSION
	@echo -n "jsthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: npm not found"
	@exit 1
endif

jsthemis_uninstall: CMD = npm uninstall jsthemis
jsthemis_uninstall:
ifdef NPM_VERSION
	@echo -n "jsthemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: jsthemis_uninstall
