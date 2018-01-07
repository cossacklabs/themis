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

THEMIS_JNI_SRC = $(wildcard jni/*.c)

THEMIS_JNI_OBJ = $(patsubst jni/%.c,$(OBJ_PATH)/jni/%.o, $(THEMIS_JNI_SRC))

THEMIS_JNI_BIN = themis_jni

ifeq ($(JDK_INCLUDE_PATH),)
	include /usr/share/java/java_defaults.mk
else
	jvm_includes=$(JAVA_HOME)/include
endif

$(OBJ_PATH)/jni/%.o: jni/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(jvm_includes) -c $< -o $@
