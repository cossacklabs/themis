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

THEMIS_JNI_OBJ = $(patsubst jni/%.c,$(OBJ_PATH)/jni/%.o, $(THEMIS_JNI_SRC))

THEMIS_JNI_BIN = themis_jni

FMT_FIXUP += $(patsubst jni/%,$(OBJ_PATH)/jni/%.fmt_fixup,$(THEMIS_JNI_SRC))
FMT_CHECK += $(patsubst jni/%,$(OBJ_PATH)/jni/%.fmt_check,$(THEMIS_JNI_SRC))

JAVA_DEFAULTS=/usr/share/java/java_defaults.mk

ifeq ($(JDK_INCLUDE_PATH),)
	ifneq ("$(wildcard $(JAVA_DEFAULTS))","")
		include $(JAVA_DEFAULTS)
	endif
else
	jvm_includes=$(JAVA_HOME)/include
endif

# Embed Themis, Soter, and cryptographic backed into shared JNI library
$(BIN_PATH)/$(LIBTHEMISJNI_SO): $(THEMIS_JNI_OBJ)
$(BIN_PATH)/$(LIBTHEMISJNI_SO): $(BIN_PATH)/$(LIBTHEMIS_A)
$(BIN_PATH)/$(LIBTHEMISJNI_SO): $(BIN_PATH)/$(LIBSOTER_A) $(SOTER_ENGINE_DEPS)

$(OBJ_PATH)/jni/%.o: jni/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(jvm_includes) -c $< -o $@

$(OBJ_PATH)/jni/%.c.fmt_fixup: \
    CMD = $(CLANG_TIDY) -fix $< -- $(CFLAGS) $(jvm_includes) 2>/dev/null && $(CLANG_FORMAT) -i $< && touch $@

$(OBJ_PATH)/jni/%.c.fmt_check: \
    CMD = $(CLANG_FORMAT) $< | diff -u $< - && $(CLANG_TIDY) $< -- $(CFLAGS) $(jvm_includes) 2>/dev/null && touch $@

$(OBJ_PATH)/jni/%.fmt_fixup: jni/%
	@mkdir -p $(@D)
	@echo -n "fixup $< "
	@$(BUILD_CMD_)

$(OBJ_PATH)/jni/%.fmt_check: jni/%
	@mkdir -p $(@D)
	@echo -n "check $< "
	@$(BUILD_CMD_)
