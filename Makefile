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

# Make sure that "all" is the default target no matter what
all:

#CC = clang

CMAKE = cmake

CLANG_FORMAT ?= clang-format
CLANG_TIDY   ?= clang-tidy
SHELL = /bin/bash

INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL) -m 644

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

UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	IS_MACOS := true
else ifeq ($(UNAME),Linux)
	IS_LINUX := true
else ifeq ($(shell uname -o),Msys)
	IS_MSYS := true
endif

ifneq ($(shell $(CC) --version 2>&1 | grep -oi "Emscripten"),)
	IS_EMSCRIPTEN := true
endif

# Detect early if we have undefined symbols due to missing exports
ifdef IS_MACOS
LDFLAGS += -Wl,-undefined,error
endif
ifdef IS_LINUX
LDFLAGS += -Wl,--no-undefined
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
ifdef IS_EMSCRIPTEN
ENGINE ?= boringssl
else
ENGINE ?= libressl
endif

# default installation paths
prefix          = $(PREFIX)
exec_prefix     = $(prefix)
bindir          = $(prefix)/bin
includedir      = $(prefix)/include
libdir          = $(exec_prefix)/lib
pkgconfigdir    = $(libdir)/pkgconfig

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

# Increment VERSION when making a new release of Themis.
#
# If you make breaking (backwards-incompatible) changes to API or ABI
# then increment LIBRARY_SO_VERSION as well, and update package names.
VERSION := $(shell test -d .git && git describe --tags || cat VERSION)
LIBRARY_SO_VERSION = 0

PHP_VERSION := $(shell php -r "echo PHP_MAJOR_VERSION;" 2>/dev/null)
RUBY_GEM_VERSION := $(shell gem --version 2>/dev/null)
RUST_VERSION := $(shell rustc --version 2>/dev/null)
GO_VERSION := $(shell which go >/dev/null 2>&1 && go version 2>&1)
NODE_VERSION := $(shell node --version 2>/dev/null)
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

ifdef IS_MACOS
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

ifdef IS_MSYS
SHARED_EXT = dll
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

# Explicitly list all exports
CFLAGS += -fvisibility=hidden

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
include src/wrappers/themis/themispp/themispp.mk
include src/wrappers/themis/wasm/wasmthemis.mk
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
soter_pkgconfig: $(BIN_PATH)/libsoter.pc
themis_static: $(BIN_PATH)/$(LIBTHEMIS_A)
themis_shared: $(BIN_PATH)/$(LIBTHEMIS_SO)
themis_pkgconfig: $(BIN_PATH)/libthemis.pc
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

install: all install_soter install_themis
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
	rsync -avz VERSION $(VERSION)
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

jsthemis_uninstall: CMD = rm -rf build/jsthemis-$(JSTHEMIS_PACKAGE_VERSION).tgz && npm uninstall jsthemis
jsthemis_uninstall:
ifdef NPM_VERSION
	@echo -n "jsthemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: phpthemis_uninstall rbthemis_uninstall themispp_uninstall jsthemis_uninstall uninstall_themis uninstall_soter
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

unpack_dist:
	@tar -xf $(THEMIS_DIST_FILENAME)

nsis_installer: $(BIN_PATH)/InstallThemis.exe

$(BIN_PATH)/InstallThemis.exe: FORCE
ifdef IS_MSYS
	@$(MAKE) install PREFIX=/ DESTDIR="$(BIN_PATH)/install"
	@ldd "$(BIN_PATH)/install/bin"/*.dll | \
	 awk '$$3 ~ "^/usr/bin" { print $$3}' | sort --uniq | \
	 xargs -I % cp % "$(BIN_PATH)/install/bin"
	@makensis Themis.nsi
	@rm -r "$(BIN_PATH)/install"
else
	@echo "NSIS installers can only be build in MSYS environment on Windows."
	@echo
	@echo "Please make sure that you are using MSYS terminal session which"
	@echo "is usually available as 'MSYS2 MSYS' shortcut in the MSYS group"
	@echo "of the Start menu."
	@exit 1
endif

FORCE:

COSSACKLABS_URL = https://www.cossacklabs.com
MAINTAINER = "Cossack Labs Limited <dev@cossacklabs.com>"
LICENSE_NAME = "Apache License Version 2.0"

DEB_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEB_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
DEB_DEPENDENCIES := --depends openssl
DEB_DEPENDENCIES_DEV += --depends "$(PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)"
DEB_DEPENDENCIES_DEV += --depends libssl-dev
DEB_DEPENDENCIES_THEMISPP = --depends "$(DEB_DEV_PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)"

RPM_DEPENDENCIES = --depends openssl
RPM_DEPENDENCIES_DEV += --depends "$(PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)"
RPM_DEPENDENCIES_DEV += --depends openssl-devel
RPM_DEPENDENCIES_THEMISPP = --depends "$(RPM_DEV_PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)"
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
DEB_DEV_PACKAGE_NAME = libthemis-dev
RPM_DEV_PACKAGE_NAME = libthemis-devel
DEB_THEMISPP_PACKAGE_NAME = libthemispp-dev
RPM_THEMISPP_PACKAGE_NAME = libthemispp-devel

PACKAGE_CATEGORY = security
SHORT_DESCRIPTION = Data security library for network communication and data storage
RPM_SUMMARY = Data security library for network communication and data storage. \
	 Themis is a data security library, providing users with high-quality security \
	 services for secure messaging of any kinds and flexible data storage. Themis \
	 is aimed at modern developers, with high level OOP wrappers for Ruby, Python, \
	 PHP, Java / Android and iOS / OSX. It is designed with ease of use in mind, \
	 high security and cross-platform availability.

POST_INSTALL_SCRIPT := $(BIN_PATH)/post_install.sh
POST_UNINSTALL_SCRIPT := $(BIN_PATH)/post_uninstall.sh

DEV_PACKAGE_FILES += $(includedir)/soter/
DEV_PACKAGE_FILES += $(includedir)/themis/
DEV_PACKAGE_FILES += $(pkgconfigdir)/

LIB_PACKAGE_FILES += $(libdir)/$(LIBSOTER_A)
LIB_PACKAGE_FILES += $(libdir)/$(LIBSOTER_SO)
LIB_PACKAGE_FILES += $(libdir)/$(LIBSOTER_LINK)
LIB_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_A)
LIB_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_SO)
LIB_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_LINK)

THEMISPP_PACKAGE_FILES += $(includedir)/themispp/

deb: DESTDIR = $(BIN_PATH)/deb/root
deb: PREFIX = /usr

deb: install themispp_install
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@printf "ldconfig" > $(POST_UNINSTALL_SCRIPT)

	@find $(DESTDIR) -name '*.$(SHARED_EXT)*' -exec strip -o {} {} \;

	@fpm --input-type dir \
		 --output-type deb \
		 --name $(DEB_DEV_PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/$(DEB_DEV_PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEB_DEPENDENCIES_DEV) \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(foreach file,$(DEV_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

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
		 $(foreach file,$(LIB_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

	@fpm --input-type dir \
		 --output-type deb \
		 --name $(DEB_THEMISPP_PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/$(DEB_THEMISPP_PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEB_DEPENDENCIES_THEMISPP) \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(foreach file,$(THEMISPP_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

	@find $(BIN_PATH) -name \*.deb

rpm: DESTDIR = $(BIN_PATH)/rpm/root
rpm: PREFIX = /usr

rpm: install themispp_install
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@printf "ldconfig" > $(POST_UNINSTALL_SCRIPT)

	@find $(DESTDIR) -name '*.$(SHARED_EXT)*' -exec strip -o {} {} \;

	@fpm --input-type dir \
         --output-type rpm \
         --name $(RPM_DEV_PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         $(RPM_DEPENDENCIES_DEV) \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         --package $(BIN_PATH)/rpm/$(RPM_DEV_PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(foreach file,$(DEV_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

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
         $(foreach file,$(LIB_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

	@fpm --input-type dir \
         --output-type rpm \
         --name $(RPM_THEMISPP_PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES_THEMISPP) \
         --package $(BIN_PATH)/rpm/$(RPM_THEMISPP_PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(foreach file,$(THEMISPP_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

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
