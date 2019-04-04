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

LIBTHEMISJNI_SO = libthemis_jni.$(SHARED_EXT)

THEMIS_JNI_SRC = $(wildcard jni/*.c)

THEMIS_JNI_OBJ = $(patsubst %,$(OBJ_PATH)/%.o, $(THEMIS_JNI_SRC))

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMIS_JNI_SRC))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMIS_JNI_SRC))

JAVA_DEFAULTS=/usr/share/java/java_defaults.mk

ifeq ($(JDK_INCLUDE_PATH),)
	ifneq ("$(wildcard $(JAVA_DEFAULTS))","")
		include $(JAVA_DEFAULTS)
	endif
else
	jvm_includes=$(JAVA_HOME)/include
endif

$(OBJ_PATH)/jni/%: CFLAGS += $(jvm_includes)

$(BIN_PATH)/$(LIBTHEMISJNI_SO): CMD = $(CC) -shared -o $@ $(filter %.o %.a, $^) $(LDFLAGS) $(CRYPTO_ENGINE_LDFLAGS)

$(BIN_PATH)/$(LIBTHEMISJNI_SO): $(THEMIS_JNI_OBJ) $(THEMIS_STATIC)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)
ifdef IS_MACOS
	@install_name_tool -id "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
	@install_name_tool -change "$(BIN_PATH)/$(notdir $@)" "$(PREFIX)/lib/$(notdir $@)" $(BIN_PATH)/$(notdir $@)
endif
