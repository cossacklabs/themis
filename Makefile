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

#CC = clang

CMAKE = cmake

CLANG_FORMAT ?= clang-format
CLANG_TIDY   ?= clang-tidy
SHELL = /bin/bash
SRC_PATH = src
ifneq ($(BUILD_PATH),)
	BIN_PATH = $(BUILD_PATH)
else
	BIN_PATH = build
endif
OBJ_PATH = $(BIN_PATH)/obj
AUD_PATH = $(BIN_PATH)/for_audit
TEST_SRC_PATH = tests
TEST_BIN_PATH = $(BIN_PATH)/tests

CFLAGS += -I$(SRC_PATH) -I$(SRC_PATH)/wrappers/themis/ -I/usr/local/include -fPIC $(CRYPTO_ENGINE_CFLAGS)
LDFLAGS += -L/usr/local/lib

unexport CFLAGS LDFLAGS

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

RIGHT_EDGE:=$(shell cols=$$(($$(tput cols) - 12)); cols=$$((cols > 68 ? 68 : cols)); echo $$cols)
MOVE_COLUMN=\033[$(RIGHT_EDGE)G

OK_STRING=$(MOVE_COLUMN)$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(MOVE_COLUMN)$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(MOVE_COLUMN)$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

PRINT_OK = printf "$@ $(OK_STRING)\n"
PRINT_OK_ = printf "$(OK_STRING)\n"
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -ne 0 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -ne 0 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;

PKGINFO_PATH = PKGINFO

UNAME=$(shell uname)

ifeq ($(UNAME),Darwin)
	IS_MACOS := true
else ifeq ($(UNAME),Linux)
	IS_LINUX := true
endif

ifneq ($(shell $(CC) --version 2>&1 | grep -oi "Emscripten"),)
	IS_EMSCRIPTEN := true
endif

ifdef IS_EMSCRIPTEN
CMAKE = emconfigure cmake
endif

define themisecho
      @tput setaf 6
      @echo $1
      @tput sgr0
endef

# default installation prefix
PREFIX ?= /usr/local

# default cryptographic engine
ENGINE ?= libressl

#engine selection block
ifneq ($(ENGINE),)
ifeq ($(ENGINE),openssl)
	CRYPTO_ENGINE_DEF = OPENSSL
	CRYPTO_ENGINE_PATH=openssl
else ifeq ($(ENGINE),libressl)
	CRYPTO_ENGINE_DEF = LIBRESSL
	CRYPTO_ENGINE_PATH=openssl
else ifeq ($(ENGINE), boringssl)
	CRYPTO_ENGINE_DEF = BORINGSSL
	CRYPTO_ENGINE_PATH=boringssl
else
	ERROR = $(error error: engine $(ENGINE) unsupported...)
endif
endif
#end of engine selection block


# search for OpenSSL headers for MacOS
ifdef IS_MACOS
	ifeq ($(CRYPTO_ENGINE_PATH),openssl)

		# if brew is installed, if openssl is installed
		PACKAGELIST = $(shell brew list | grep -om1 '^openssl')
		ifeq ($(PACKAGELIST),openssl)

		 	# path to openssl (usually "/usr/local/opt/openssl")
			OPENSSL_PATH := $(shell brew --prefix openssl)
			ifneq ($(OPENSSL_PATH),)
				CRYPTO_ENGINE_INCLUDE_PATH = $(OPENSSL_PATH)/include
				CRYPTO_ENGINE_LIB_PATH = $(OPENSSL_PATH)/lib
			endif
		endif
	endif
endif

CRYPTO_ENGINE = $(SRC_PATH)/soter/$(CRYPTO_ENGINE_PATH)
CFLAGS += -D$(CRYPTO_ENGINE_DEF) -DCRYPTO_ENGINE_PATH=$(CRYPTO_ENGINE_PATH)

ifneq ($(ENGINE_INCLUDE_PATH),)
	CRYPTO_ENGINE_INCLUDE_PATH = $(ENGINE_INCLUDE_PATH)
endif
ifneq ($(ENGINE_LIB_PATH),)
	CRYPTO_ENGINE_LIB_PATH = $(ENGINE_LIB_PATH)
endif
ifneq ($(AUTH_SYM_ALG),)
	CFLAGS += -D$(AUTH_SYM_ALG)
endif
ifneq ($(SYM_ALG),)
	CFLAGS += -D$(SYM_ALG)
endif

ifeq ($(RSA_KEY_LENGTH),1024)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_1024
endif

ifeq ($(RSA_KEY_LENGTH),2048)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_2048
endif

ifeq ($(RSA_KEY_LENGTH),4096)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_4096
endif

ifeq ($(RSA_KEY_LENGTH),8192)
	CFLAGS += -DTHEMIS_RSA_KEY_LENGTH=RSA_KEY_LENGTH_8192
endif

DEFAULT_VERSION := 0.11.0
GIT_VERSION := $(shell if [ -d ".git" ]; then git version; fi 2>/dev/null)
# check that repo has any tag
GIT_TAG_STATUS := $(shell git describe --tags HEAD 2>/dev/null)
GIT_TAG_STATUS := $(.SHELLSTATUS)

ifdef GIT_VERSION
# if has tag then use it
        ifeq ($(GIT_TAG_STATUS),0)
# <tag>-<commit_count_after_tag>-<last_commit-hash>
                VERSION = $(shell git describe --tags HEAD | cut -b 1-)
        else
# <base_version>-<total_commit_count>-<last_commit_hash>
                VERSION = $(DEFAULT_VERSION)-$(shell git rev-list --all --count)-$(shell git describe --always HEAD)
        endif
else
# if it's not git repo then use date as version
        VERSION = $(shell date -I | sed s/-/_/g)
endif

PHP_VERSION := $(shell php -r "echo PHP_MAJOR_VERSION;" 2>/dev/null)
RUBY_GEM_VERSION := $(shell gem --version 2>/dev/null)
RUST_VERSION := $(shell rustc --version 2>/dev/null)
GO_VERSION := $(shell which go >/dev/null 2>&1 && go version 2>&1)
NPM_VERSION := $(shell npm --version 2>/dev/null)
PIP_VERSION := $(shell pip --version 2>/dev/null)
PYTHON2_VERSION := $(shell which python2 >/dev/null 2>&1 && python2 --version 2>&1)
PYTHON3_VERSION := $(shell python3 --version 2>/dev/null)
ifdef PIP_VERSION
PIP_THEMIS_INSTALL := $(shell pip freeze |grep themis)
endif
ifneq ("$(wildcard src/wrappers/themis/php/Makefile)","")
PHP_THEMIS_INSTALL = 1
endif

SHARED_EXT = so

ifeq ($(shell uname),Darwin)
SHARED_EXT = dylib
ifneq ($(SDK),)
SDK_PLATFORM_VERSION=$(shell xcrun --sdk $(SDK) --show-sdk-platform-version)
XCODE_BASE=$(shell xcode-select --print-path)
CC=$(XCODE_BASE)/usr/bin/gcc
BASE=$(shell xcrun --sdk $(SDK) --show-sdk-platform-path)
SDK_BASE=$(shell xcrun --sdk $(SDK) --show-sdk-path)
FRAMEWORKS=$(SDK_BASE)/System/Library/Frameworks/
SDK_INCLUDES=$(SDK_BASE)/usr/include
CFLAFS += -isysroot $(SDK_BASE)
endif
ifneq ($(ARCH),)
CFLAFS += -arch $(ARCH)
endif
endif

ifneq ($(shell $(CC) --version 2>&1 | grep -E -i -c "clang version"),0)
	IS_CLANG_COMPILER := true
endif

ifdef COVERAGE
	CFLAGS += -g -O0 --coverage
	LDFLAGS += --coverage
endif

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

ifneq ($(GEM_INSTALL_OPTIONS),)
	_GEM_INSTALL_OPTIONS = $(GEM_INSTALL_OPTIONS)
endif

define supported =
$(shell if echo "int main(void){}" | $(if $(2),$(2),$(CC)) -x c -fsyntax-only -Werror $(1) - >/dev/null 2>&1; then echo "yes"; fi)
endef

# Treat warnings as errors if requested
ifeq (yes,$(WITH_FATAL_WARNINGS))
CFLAGS += -Werror
endif

# We are security-oriented so we use a pretty paranoid set of flags
# by default. For starters, enable default set of warnings.
CFLAGS += -Wall -Wextra
# Various security-related diagnostics for printf/scanf family
CFLAGS += -Wformat
CFLAGS += -Wformat-nonliteral
ifeq (yes,$(call supported,-Wformat-overflow))
CFLAGS += -Wformat-overflow
endif
CFLAGS += -Wformat-security
ifeq (yes,$(call supported,-Wformat-signedness))
CFLAGS += -Wformat-signedness
endif
ifeq (yes,$(call supported,-Wformat-truncation))
CFLAGS += -Wformat-truncation
endif
# Warn about possible undefined behavior
ifeq (yes,$(call supported,-Wnull-dereference))
CFLAGS += -Wnull-dereference
endif
ifeq (yes,$(call supported,-Wshift-overflow))
CFLAGS += -Wshift-overflow
endif
ifeq (yes,$(call supported,-Wshift-negative-value))
CFLAGS += -Wshift-negative-value
endif
CFLAGS += -Wstrict-overflow
# Ensure full coverage of switch-case branches
CFLAGS += -Wswitch
# Forbid alloca() and variable-length arrays
ifeq (yes,$(call supported,-Walloca))
CFLAGS += -Walloca
endif
CFLAGS += -Wvla
# Forbid pointer arithmetic with "void*" type
CFLAGS += -Wpointer-arith
# Forbid old-style C function prototypes
# (skip for C++ files as older g++ complains about it)
ifeq (yes,$(call supported,-Wstrict-prototypes))
CFLAGS += $(if $(findstring .cpp,$(suffix $<)),,-Wstrict-prototypes)
endif

# strict checks for docs
#CFLAGS += -Wdocumentation -Wno-error=documentation

# fixing compatibility between x64 0.9.6 and x64 0.10.0
# https://github.com/cossacklabs/themis/pull/279
ifeq ($(NO_SCELL_COMPAT),)
	CFLAGS += -DSCELL_COMPAT
endif

ifndef ERROR
include src/soter/soter.mk
include src/themis/themis.mk
ifndef CARGO
include src/wrappers/themis/jsthemis/jsthemis.mk
include jni/themis_jni.mk
endif
endif

JSTHEMIS_PACKAGE_VERSION=$(shell cat src/wrappers/themis/jsthemis/package.json \
  | grep version \
  | head -1 \
  | awk -F: '{ print $$2 }' \
  | sed 's/[",]//g' \
  | tr -d '[[:space:]]')

all: err themis_static soter_static themis_shared soter_shared themis_pkgconfig soter_pkgconfig
	@echo $(VERSION)

soter_static:  $(BIN_PATH)/$(LIBSOTER_A)
soter_shared:  $(BIN_PATH)/$(LIBSOTER_SO)
themis_static: $(BIN_PATH)/$(LIBTHEMIS_A)
themis_shared: $(BIN_PATH)/$(LIBTHEMIS_SO)
themis_jni:    $(BIN_PATH)/$(LIBTHEMISJNI_SO)

#
# Common build rules
#

$(OBJ_PATH)/%.c.o: CMD = $(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_PATH)/%.c.o: %.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(OBJ_PATH)/%.cpp.o: CMD = $(CXX) -c -o $@ $< $(CFLAGS)

$(OBJ_PATH)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(OBJ_PATH)/%.c.fmt_fixup $(OBJ_PATH)/%.h.fmt_fixup $(OBJ_PATH)/%.cpp.fmt_fixup $(OBJ_PATH)/%.hpp.fmt_fixup: \
    CMD = $(CLANG_TIDY) -fix $< -- $(CFLAGS) 2>/dev/null && $(CLANG_FORMAT) -i $< && touch $@

$(OBJ_PATH)/%.c.fmt_check $(OBJ_PATH)/%.h.fmt_check $(OBJ_PATH)/%.cpp.fmt_check $(OBJ_PATH)/%.hpp.fmt_check: \
    CMD = $(CLANG_FORMAT) $< | diff -u $< - && $(CLANG_TIDY) $< -- $(CFLAGS) 2>/dev/null && touch $@

$(OBJ_PATH)/%.fmt_fixup: %
	@mkdir -p $(@D)
	@echo -n "fixup $< "
	@$(BUILD_CMD_)

$(OBJ_PATH)/%.fmt_check: %
	@mkdir -p $(@D)
	@echo -n "check $< "
	@$(BUILD_CMD_)

THEMISPP_HEADERS = $(wildcard $(SRC_PATH)/wrappers/themis/themispp/*.hpp)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMISPP_HEADERS))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMISPP_HEADERS))

#$(AUD_PATH)/%: CMD = $(CC) $(CFLAGS) -E -dI -dD $< -o $@
$(AUD_PATH)/%: CMD = ./scripts/pp.sh  $< $@

$(AUD_PATH)/%: $(SRC_PATH)/%
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

ifndef CARGO
include tests/test.mk
include tools/afl/fuzzy.mk
endif

err: ; $(ERROR)

fmt: $(FMT_FIXUP)
fmt_check: $(FMT_CHECK)

clean: CMD = rm -rf $(BIN_PATH)

clean: nist_rng_test_suite_clean clean_rust
	@$(BUILD_CMD)

clean_rust:
ifdef RUST_VERSION
	@cargo clean
	@rm -f tools/rust/*.rust
endif

make_install_dirs: CMD = mkdir -p $(PREFIX)/include/themis $(PREFIX)/include/soter $(PREFIX)/lib $(PREFIX)/lib/pkgconfig

make_install_dirs:
	@echo -n "making dirs for install "
	@$(BUILD_CMD_)

install_soter_headers: CMD = install $(SRC_PATH)/soter/*.h $(PREFIX)/include/soter

install_soter_headers: err all make_install_dirs
	@echo -n "install soter headers "
	@$(BUILD_CMD_)

install_themis_headers: CMD = install $(SRC_PATH)/themis/*.h $(PREFIX)/include/themis

install_themis_headers: err all make_install_dirs
	@echo -n "install themis headers "
	@$(BUILD_CMD_)

install_static_libs: CMD = install $(BIN_PATH)/*.a $(PREFIX)/lib

install_static_libs: err all make_install_dirs
	@echo -n "install static libraries "
	@$(BUILD_CMD_)

install_shared_libs: CMD = install $(BIN_PATH)/*.$(SHARED_EXT) $(PREFIX)/lib

install_shared_libs: err all make_install_dirs
	@echo -n "install shared libraries "
	@$(BUILD_CMD_)

install_pkgconfig: CMD = install $(BIN_PATH)/*.pc $(PREFIX)/lib/pkgconfig

install_pkgconfig: err all make_install_dirs
	@echo -n "install pkg-config files "
	@$(BUILD_CMD_)

install: install_soter_headers install_themis_headers install_static_libs install_shared_libs install_pkgconfig
	@echo -n "Themis installed to $(PREFIX)"
	@$(PRINT_OK_)
ifdef IS_LINUX
	-@ldconfig
endif
	@if [ -e /usr/include/themis/themis.h ] && [ -e /usr/local/include/themis/themis.h ]; then \
	     echo ""; \
	     echo "Multiple Themis installations detected in standard system paths:"; \
	     echo ""; \
	     echo "  - /usr"; \
	     echo "  - /usr/local"; \
	     echo ""; \
	     echo "This may lead to surprising behaviour when building and using software"; \
	     echo "which depends on Themis."; \
	     echo ""; \
	     echo "If you previously had Themis installed from source to \"/usr\","; \
	     echo "consider uninstalling the old version with"; \
	     echo ""; \
	     echo "    sudo $(MAKE) uninstall PREFIX=/usr"; \
	     echo ""; \
	     echo "and keep the new version in \"/usr/local\"."; \
	     echo ""; \
	 fi

get_version:
	@echo $(VERSION)

THEMIS_DIST_FILENAME = $(VERSION).tar.gz

dist:
	mkdir -p $(VERSION)
	rsync -avz src $(VERSION)
	rsync -avz docs $(VERSION)
	rsync -avz gothemis $(VERSION)
	rsync -avz gradle $(VERSION)
	rsync -avz jni $(VERSION)
	rsync -avz --exclude 'tests/soter/nist-sts/assess' tests $(VERSION)
	rsync -avz tools $(VERSION)
	rsync -avz CHANGELOG.md $(VERSION)
	rsync -avz LICENSE $(VERSION)
	rsync -avz Makefile $(VERSION)
	rsync -avz README.md $(VERSION)
	rsync -avz build.gradle $(VERSION)
	rsync -avz gradlew $(VERSION)
	rsync -avz themis.podspec $(VERSION)
	tar -zcvf $(THEMIS_DIST_FILENAME) $(VERSION)
	rm -rf $(VERSION)

for-audit: $(SOTER_AUD) $(THEMIS_AUD)


phpthemis_uninstall: CMD = if [ -e src/wrappers/themis/php/Makefile ]; then cd src/wrappers/themis/php && make distclean ; fi;
phpthemis_uninstall:
ifdef PHP_THEMIS_INSTALL
	@echo -n "phpthemis uninstall "
	@$(BUILD_CMD_)
endif

rbthemis_uninstall: CMD = gem uninstall themis
rbthemis_uninstall:
ifdef RUBY_GEM_VERSION
	@echo -n "rbthemis uninstall "
	@$(BUILD_CMD_)
endif

rubythemis_uninstall_deprecation_warning:
	@printf "The rubythemis gem is deprecated. Please use rbthemis_uninstall$(WARN_STRING)\n"
	@printf "target instead of rubythemis_uninstall.$(WARN_STRING)\n"

rubythemis_uninstall: rubythemis_uninstall_deprecation_warning rbthemis_uninstall


jsthemis_uninstall: CMD = rm -rf build/jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz && npm uninstall jsthemis
jsthemis_uninstall:
ifdef NPM_VERSION
	@echo -n "jsthemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: CMD = rm -rf $(PREFIX)/include/themis && rm -rf $(PREFIX)/include/soter && rm -f $(PREFIX)/lib/libsoter.a && rm -f $(PREFIX)/lib/libthemis.a && rm -f $(PREFIX)/lib/libsoter.$(SHARED_EXT) && rm -f $(PREFIX)/lib/libthemis.$(SHARED_EXT) && rm -f $(PREFIX)/lib/pkgconfig/libsoter.pc && rm -f $(PREFIX)/lib/pkgconfig/libthemis.pc

uninstall: phpthemis_uninstall rbthemis_uninstall themispp_uninstall jsthemis_uninstall
	@echo -n "Themis uninstalled from $(PREFIX) "
	@$(BUILD_CMD_)

ifeq ($(PHP_VERSION),5)
    PHP_FOLDER = php
else
    PHP_FOLDER = php7
endif

phpthemis_install: CMD = cd src/wrappers/themis/$(PHP_FOLDER) && phpize && ./configure && make install

phpthemis_install:
ifdef PHP_VERSION
	@echo -n "phpthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: php not found"
	@exit 1
endif

rbthemis_install: CMD = cd src/wrappers/themis/ruby && gem build rbthemis.gemspec && gem install ./*.gem $(_GEM_INSTALL_OPTIONS)

rbthemis_install:
ifdef RUBY_GEM_VERSION
	@echo -n "rbthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: ruby gem not found"
	@exit 1
endif

rubythemis_install_deprecation_warning:
	@printf "The rubythemis gem is deprecated. Please use rbthemis_install$(WARN_STRING)\n"
	@printf "target instead of rubythemis_install.$(WARN_STRING)\n"

rubythemis_install: rubythemis_install_deprecation_warning rbthemis_install

jsthemis_install: CMD = cd src/wrappers/themis/jsthemis && npm pack && mv jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz ../../../../build && cd - && npm install nan && npm install ./build/jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz
jsthemis_install:
ifdef NPM_VERSION
	@echo -n "jsthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: npm not found"
	@exit 1
endif

pythemis_install: CMD = cd src/wrappers/themis/python/ && python2 setup.py install --record files.txt;  python3 setup.py install --record files3.txt
pythemis_install:
ifeq ($(or $(PYTHON2_VERSION),$(PYTHON3_VERSION)),)
	@echo "python2 or python3 not found"
	@exit 1
endif
	@echo -n "pythemis install "
	@$(BUILD_CMD_)


themispp_install: CMD = install $(SRC_PATH)/wrappers/themis/themispp/*.hpp $(PREFIX)/include/themispp

themispp_install:
	@mkdir -p $(PREFIX)/include/themispp
	@$(BUILD_CMD)

themispp_uninstall: CMD = rm -rf $(PREFIX)/include/themispp

themispp_uninstall:
	@echo -n "themispp uninstall "
	@$(BUILD_CMD_)

soter_collect_headers:
	@mkdir -p $(BIN_PATH)/include/soter
	@cd src/soter && find . -name \*.h -exec cp --parents {} ../../$(BIN_PATH)/include/soter/ \; && cd - > /dev/null

themis_collect_headers:
	@mkdir -p $(BIN_PATH)/include/themis
	@cd src/themis && find . -name \*.h -exec cp --parents {} ../../$(BIN_PATH)/include/themis/ \; && cd - > /dev/null

collect_headers: themis_collect_headers soter_collect_headers

unpack_dist:
	@tar -xf $(THEMIS_DIST_FILENAME)


COSSACKLABS_URL = https://www.cossacklabs.com
MAINTAINER = "Cossack Labs Limited <dev@cossacklabs.com>"
# tag version from VCS
VERSION := $(shell git describe --tags HEAD | cut -b 1-)
LICENSE_NAME = "Apache License Version 2.0"

LIBRARY_SO_VERSION := $(shell echo $(VERSION) | sed 's/^\([0-9.]*\)\(.*\)*$$/\1/')
ifeq ($(LIBRARY_SO_VERSION),)
	LIBRARY_SO_VERSION := $(DEFAULT_VERSION)
endif

DEB_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEB_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
DEB_DEPENDENCIES := --depends openssl
DEB_DEPENDENCIES_DEV := $(DEB_DEPENDENCIES) --depends libssl-dev

RPM_DEPENDENCIES = --depends openssl
RPM_DEPENDENCIES_DEV := $(RPM_DEPENDENCIES) --depends openssl-devel
RPM_RELEASE_NUM = 1

ifeq ($(shell lsb_release -is 2> /dev/null),Debian)
#0.9.4-153-g9915004+jessie_amd64.deb.
	NAME_SUFFIX = $(VERSION)+$(DEB_CODENAME)_$(DEB_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else ifeq ($(shell lsb_release -is 2> /dev/null),Ubuntu)
	NAME_SUFFIX = $(VERSION)+$(DEB_CODENAME)_$(DEB_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else
# centos/rpm
	OS_NAME = $(shell cat /etc/os-release | grep -e "^ID=\".*\"" | cut -d'"' -f2)
	OS_VERSION = $(shell cat /etc/os-release | grep -i version_id|cut -d'"' -f2)
	ARCHITECTURE = $(shell arch)
	RPM_VERSION = $(shell echo -n "$(VERSION)"|sed s/-/_/g)
	NAME_SUFFIX = $(RPM_VERSION).$(OS_NAME)$(OS_VERSION).$(ARCHITECTURE).rpm
endif

PACKAGE_NAME = libthemis
PACKAGE_CATEGORY = security
SHORT_DESCRIPTION = Data security library for network communication and data storage
RPM_SUMMARY = Data security library for network communication and data storage. \
	 Themis is a data security library, providing users with high-quality security \
	 services for secure messaging of any kinds and flexible data storage. Themis \
	 is aimed at modern developers, with high level OOP wrappers for Ruby, Python, \
	 PHP, Java / Android and iOS / OSX. It is designed with ease of use in mind, \
	 high security and cross-platform availability.

HEADER_DIRS = $(shell ls $(BIN_PATH)/include)

HEADER_FILES_MAP = $(foreach dir,$(HEADER_DIRS), $(BIN_PATH)/include/$(dir)/=$(PREFIX)/include/$(dir))

STATIC_LIBRARY_FILES = $(shell ls $(BIN_PATH)/ | egrep *\.a$$)
STATIC_BINARY_LIBRARY_MAP = $(foreach file,$(STATIC_LIBRARY_FILES),$(strip $(BIN_PATH)/$(file)=$(PREFIX)/lib/$(file) $(BIN_PATH)/$(file).$(LIBRARY_SO_VERSION)=$(PREFIX)/lib/$(file).$(LIBRARY_SO_VERSION)))

SHARED_LIBRARY_FILES = $(shell ls $(BIN_PATH)/ | egrep *\.$(SHARED_EXT)$$)
SHARED_BINARY_LIBRARY_MAP = $(foreach file,$(SHARED_LIBRARY_FILES),$(strip $(BIN_PATH)/$(file).$(LIBRARY_SO_VERSION)=$(PREFIX)/lib/$(file).$(LIBRARY_SO_VERSION) $(BIN_PATH)/$(file)=$(PREFIX)/lib/$(file)))

PKGCONFIG_FILES = $(shell ls $(BIN_PATH)/ | egrep *\.pc$$)
PKGCONFIG_MAP = $(foreach file,$(PKGCONFIG_FILES),$(strip $(BIN_PATH)/$(file)=$(PREFIX)/lib/pkgconfig/$(file)))

BINARY_LIBRARY_MAP = $(strip $(STATIC_BINARY_LIBRARY_MAP) $(SHARED_BINARY_LIBRARY_MAP))

POST_INSTALL_SCRIPT := $(BIN_PATH)/post_install.sh
POST_UNINSTALL_SCRIPT := $(BIN_PATH)/post_uninstall.sh

install_shell_scripts:
# run ldconfig to update ld.$(SHARED_EXT) cache
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@cp $(POST_INSTALL_SCRIPT) $(POST_UNINSTALL_SCRIPT)

symlink_realname_to_soname:
	# add version to filename and create symlink with realname to full name of library
	@for f in `ls $(BIN_PATH) | egrep ".*\.(so|a)(\..*)?$$" | tr '\n' ' '`; do \
		mv $(BIN_PATH)/$$f $(BIN_PATH)/$$f.$(LIBRARY_SO_VERSION); \
		ln -s $(PREFIX)/lib/$$f.$(LIBRARY_SO_VERSION) $(BIN_PATH)/$$f; \
	done


strip:
	@find . -name \*.$(SHARED_EXT)\.* -exec strip -o {} {} \;

deb: PREFIX = /usr

deb: soter_static themis_static soter_shared themis_shared soter_pkgconfig themis_pkgconfig collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/deb

#libPACKAGE-dev
	@fpm --input-type dir \
		 --output-type deb \
		 --name $(PACKAGE_NAME)-dev \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/$(PACKAGE_NAME)-dev_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEB_DEPENDENCIES_DEV) --depends "$(PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)" \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(HEADER_FILES_MAP) $(PKGCONFIG_MAP)

#libPACKAGE
	@fpm --input-type dir \
		 --output-type deb \
		 --name $(PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/$(PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEB_DEPENDENCIES) \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --deb-priority optional \
		 --category $(PACKAGE_CATEGORY) \
		 $(BINARY_LIBRARY_MAP)

# it's just for printing .deb files
	@find $(BIN_PATH) -name \*.deb

rpm: PREFIX = /usr

rpm: themis_static themis_shared themis_pkgconfig soter_static soter_shared soter_pkgconfig collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/rpm
#libPACKAGE-devel
	@fpm --input-type dir \
         --output-type rpm \
         --name $(PACKAGE_NAME)-devel \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         $(RPM_DEPENDENCIES_DEV) --depends "$(PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)" \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         --package $(BIN_PATH)/rpm/$(PACKAGE_NAME)-devel-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
           $(HEADER_FILES_MAP) $(PKGCONFIG_MAP)

#libPACKAGE
	@fpm --input-type dir \
         --output-type rpm \
         --name $(PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES) \
         --package $(BIN_PATH)/rpm/$(PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(BINARY_LIBRARY_MAP)
# it's just for printing .rpm files
	@find $(BIN_PATH) -name \*.rpm

define PKGINFO
PACKAGE=$(PACKAGE_NAME)
SECTION=$(PACKAGE_CATEGORY)
MAINTAINER=$(MAINTAINER)
VERSION=$(VERSION)
HOMEPAGE=$(COSSACKLABS_URL)
LICENSE=$(LICENSE_NAME)
DESCRIPTION="$(SHORT_DESCRIPTION)"
endef
export PKGINFO
pkginfo:
	@echo "$$PKGINFO" > $(PKGINFO_PATH)

PHP_VERSION_FULL:=$(shell php -r "echo PHP_MAJOR_VERSION.'.'.PHP_MINOR_VERSION;" 2>/dev/null)
ifeq ($(OS_CODENAME),jessie)
    PHP_DEPENDENCIES:=php5
else
    PHP_DEPENDENCIES:=php$(PHP_VERSION_FULL)
endif

PHP_PACKAGE_NAME:=libphpthemis-php$(PHP_VERSION_FULL)
PHP_POST_INSTALL_SCRIPT:=./scripts/phpthemis_postinstall.sh
PHP_PRE_UNINSTALL_SCRIPT:=./scripts/phpthemis_preuninstall.sh
PHP_API:=$(shell php -i 2>/dev/null|grep 'PHP API'|sed 's/PHP API => //')
PHP_LIB_MAP:=./src/wrappers/themis/$(PHP_FOLDER)/.libs/phpthemis.so=/usr/lib/php/$(PHP_API)/

deb_php:
	@mkdir -p $(BIN_PATH)/deb
	@fpm --input-type dir \
		 --output-type deb \
		 --name $(PHP_PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --package $(BIN_PATH)/deb/$(PHP_PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 --depends "$(PHP_DEPENDENCIES)" \
		 --deb-priority optional \
		 --after-install $(PHP_POST_INSTALL_SCRIPT) \
		 --before-remove $(PHP_PRE_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 --deb-no-default-config-files \
		 $(PHP_LIB_MAP)
	@find $(BIN_PATH) -name $(PHP_PACKAGE_NAME)\*.deb

php_info:
	@echo "PHP_VERSION_FULL: $(PHP_VERSION_FULL)"
	@echo "PHP_API: $(PHP_API)"
	@echo "PHP_PACKAGE_NAME: $(PHP_PACKAGE_NAME)"
	@echo "PHP_FOLDER: $(PHP_FOLDER)"
