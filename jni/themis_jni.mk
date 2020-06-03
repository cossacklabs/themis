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
	else
		ifndef JAVA_HOME
			JAVA_HOME := $(shell java -XshowSettings:properties -version 2>&1 | sed -n '/java.home/s/.*java.home = //p')
		endif
		jvm_includes += -I$(JAVA_HOME)/include
		ifdef IS_LINUX
			jvm_includes += -I$(JAVA_HOME)/include/linux
			# On some systems (like CentOS) "java.home" value reported by Java
			# points to "/usr/lib/jvm/${java_version}/jre" when the real Java
			# home is located in "/usr/lib/jvm/${java_version}" and JDK headers
			# are located in "/usr/lib/jvm/${java_version}/include". Workaround:
			# look one directory above the apparent Java home too.
			jvm_includes += -I$(JAVA_HOME)/../include
			jvm_includes += -I$(JAVA_HOME)/../include/linux
		endif
		ifdef IS_MACOS
			jvm_includes += -I$(JAVA_HOME)/include/darwin
		endif
	endif
else
	jvm_includes += $(addprefix -I,$(JDK_INCLUDE_PATH))
endif

$(OBJ_PATH)/jni/%: CFLAGS += $(jvm_includes)

$(BIN_PATH)/$(LIBTHEMISJNI_SO): CMD = $(CC) -shared -o $@ $(filter %.o %.a, $^) $(LDFLAGS) -lthemis

$(BIN_PATH)/$(LIBTHEMISJNI_SO): $(THEMIS_JNI_OBJ) $(BIN_PATH)/$(LIBTHEMIS_SO)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

themis_jni_install: $(BIN_PATH)/$(LIBTHEMISJNI_SO)
	@echo -n "install Themis JNI "
	@mkdir -p $(DESTDIR)$(jnidir)
	@$(INSTALL_PROGRAM) $(BIN_PATH)/$(LIBTHEMISJNI_SO) $(DESTDIR)$(jnidir)
ifdef IS_MACOS
	@install_name_tool -id "$(jnidir)/$(LIBTHEMISJNI_SO)" "$(DESTDIR)$(jnidir)/$(LIBTHEMISJNI_SO)"
	@install_name_tool -change "$(BIN_PATH)/$(LIBTHEMIS_SO)" "$(libdir)/$(LIBTHEMIS_SO)" "$(DESTDIR)$(jnidir)/$(LIBTHEMISJNI_SO)"
endif
	@$(PRINT_OK_)
	@[ $(MODE_PACKAGING) -eq 1 ] || (java_library_path=$$(\
	    java -XshowSettings:properties -version 2>&1 \
	    | sed -E 's/^ +[^=]+ =/_&/' \
	    | awk -v prop=java.library.path \
	      'BEGIN { RS = "_"; IFS = " = " } \
	       { if($$1 ~ prop) { \
	           for (i = 3; i <= NF; i++) { \
	             print $$i \
	           } \
	         } \
	       }' \
	 ) && \
	 jnidir=$$(cd "$(jnidir)" && pwd) && \
	 if ! echo "$$java_library_path" | grep -q "^$${jnidir}$$"; \
	 then \
	     echo ''; \
	     echo "Your Java installation does not seem to have \"$${jnidir}\" in its"; \
	     echo 'search path for JNI libraries:'; \
	     echo ''; \
	     echo "$$java_library_path" | sed 's/^/    /'; \
	     echo ''; \
	     echo 'You will need to either add it to the "java.library.path" property'; \
	     echo 'on application startup, or to move $(LIBTHEMISJNI_SO) manually'; \
	     echo 'to one of these locations so that Java could find it.'; \
	     echo ''; \
	 fi)

themis_jni_uninstall:
	@echo -n "uninstall Themis JNI "
	@rm  -f $(DESTDIR)$(jnidir)/$(LIBTHEMISJNI_SO)
	@$(PRINT_OK_)

uninstall: themis_jni_uninstall
