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

#===== Early setup =============================================================

# Set default goal for "make"
.DEFAULT_GOAL := all

# Set shell for target commands
SHELL = bash

# Disable built-in rules
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

## build directory
BUILD_PATH ?= build

# Include system configuration file, creating it if necessary
-include $(BUILD_PATH)/configure.mk

$(BUILD_PATH)/configure.mk:
	@./configure

#===== Variables ===============================================================

#----- Versioning --------------------------------------------------------------

# Increment VERSION when making a new release of Themis.
#
# If you make breaking (backwards-incompatible) changes to API or ABI
# then increment LIBRARY_SO_VERSION as well, and update package names.
VERSION := $(shell test -d .git && git describe --tags || cat VERSION)
LIBRARY_SO_VERSION = 0

#----- Toolchain ---------------------------------------------------------------

CMAKE ?= cmake
GO    ?= go

CLANG_FORMAT ?= clang-format
CLANG_TIDY   ?= clang-tidy

INSTALL         ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA    ?= $(INSTALL) -m 644

#----- Build directories -------------------------------------------------------

INC_PATH = include
SRC_PATH = src
BIN_PATH = $(BUILD_PATH)
OBJ_PATH = $(BIN_PATH)/obj
AUD_PATH = $(BIN_PATH)/for_audit

TEST_SRC_PATH = tests
TEST_BIN_PATH = $(BIN_PATH)/tests

#----- Installation paths ------------------------------------------------------

## installation prefix
PREFIX ?= /usr/local

# Advanced variables for fine-tuning installation paths
prefix       ?= $(PREFIX)
exec_prefix  ?= $(prefix)
bindir       ?= $(prefix)/bin
includedir   ?= $(prefix)/include
libdir       ?= $(exec_prefix)/lib
jnidir       ?= $(libdir)
pkgconfigdir ?= $(libdir)/pkgconfig

#----- Basic compiler flags ----------------------------------------------------

# Add Themis source directory to search paths
CFLAGS  += -I$(INC_PATH) -I$(SRC_PATH) -I$(SRC_PATH)/wrappers/themis/
LDFLAGS += -L$(BIN_PATH)
# Not all platforms include /usr/local in default search path
CFLAGS  += -I/usr/local/include
LDFLAGS += -L/usr/local/lib
# Build shared libraries
CFLAGS  += -fPIC

########################################################################
#
# Pretty-printing utilities
#

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

########################################################################
#
# Select and configure cryptographic engine
#

ifdef IS_EMSCRIPTEN
ENGINE ?= boringssl
else
ENGINE ?= libressl
endif

ifeq ($(ENGINE),openssl)
	CRYPTO_ENGINE_DEF  = OPENSSL
	CRYPTO_ENGINE_PATH = openssl
else ifeq ($(ENGINE),libressl)
	CRYPTO_ENGINE_DEF  = LIBRESSL
	CRYPTO_ENGINE_PATH = openssl
else ifeq ($(ENGINE),boringssl)
	CRYPTO_ENGINE_DEF  = BORINGSSL
	CRYPTO_ENGINE_PATH = boringssl
else
$(error engine $(ENGINE) is not supported)
endif

CRYPTO_ENGINE = $(SRC_PATH)/soter/$(CRYPTO_ENGINE_PATH)
CFLAGS += -D$(CRYPTO_ENGINE_DEF) -DCRYPTO_ENGINE_PATH=$(CRYPTO_ENGINE_PATH)
CFLAGS += $(CRYPTO_ENGINE_CFLAGS)

# If we're building for macOS and there's Homebrew installed then prefer
# Homebrew's OpenSSL instead of the system one by default.
ifdef IS_MACOS
	ifeq ($(CRYPTO_ENGINE_PATH),openssl)
		ifneq ($(HOMEBREW_OPENSSL_PATH),)
			CRYPTO_ENGINE_INCLUDE_PATH = $(HOMEBREW_OPENSSL_PATH)/include
			CRYPTO_ENGINE_LIB_PATH = $(HOMEBREW_OPENSSL_PATH)/lib
		endif
	endif
endif

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

########################################################################
#
# Compilation flags for C/C++ code
#

# Some build systems may call C compilers themselves. We don't want to leak
# our compilation flags there, so do not export these variables.
unexport CFLAGS LDFLAGS

# Prevent undefined symbols in produced binaries, but allow them for sanitizers
# which expect the libraries linked into the main executable to be underlinked.
ifndef WITH_ASAN
ifndef WITH_MSAN
ifndef WITH_TSAN
ifndef WITH_UBSAN
# Not all Emscripten toolchains support these flags so leave them out as well.
ifndef IS_EMSCRIPTEN
ifdef IS_MACOS
LDFLAGS += -Wl,-undefined,error
endif
ifdef IS_LINUX
LDFLAGS += -Wl,--no-undefined
endif
endif
endif
endif
endif
endif

CFLAGS += -O2 -g
# Get better runtime backtraces by preserving the frame pointer. This eats
# one of seven precious registers on x86, but our functions are quite large
# so they almost always use stack and need the frame pointer anyway.
CFLAGS += -fno-omit-frame-pointer
# Enable runtime stack canaries for functions to guard for buffer overflows.
# FIXME(ilammy, 2020-10-29): enable stack canaries for WasmThemis too
# Currently, stack protector is not supported by the "upstream" flavor
# of Emscripten toolchain. Tracking issue is here:
# https://github.com/emscripten-core/emscripten/issues/9780
ifndef IS_EMSCRIPTEN
ifeq (yes,$(call supported,-fstack-protector-strong))
CFLAGS += -fstack-protector-strong
else
CFLAGS += -fstack-protector
endif
endif
# Enable miscellaneous compile-time checks in standard library usage.
CFLAGS += -D_FORTIFY_SOURCE=2
# Prevent global offset table overwrite attacks.
ifdef IS_LINUX
LDFLAGS += -Wl,-z,relro -Wl,-z,now
endif

ifdef COVERAGE
	CFLAGS += -O0 --coverage
	LDFLAGS += --coverage
endif

ifdef DEBUG
	CFLAGS += -O0 -DDEBUG
endif

ifneq ($(GEM_INSTALL_OPTIONS),)
	_GEM_INSTALL_OPTIONS = $(GEM_INSTALL_OPTIONS)
endif

define supported =
$(shell if echo "int main(void){}" | $(if $(AFL_CC),$(AFL_CC),$(CC)) -x c -fsyntax-only -Werror $(1) - >/dev/null 2>&1; then echo "yes"; fi)
endef

ifeq (yes,$(WITH_FATAL_WARNINGS))
CFLAGS += -Werror
endif

# We are security-oriented so we use a pretty paranoid^W comprehensive set
# of compiler flags. Some of them are not available for all compilers, so
# we have to check if we can use them first.
CFLAGS += -Wall -Wextra
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
CFLAGS += -Wswitch
ifeq (yes,$(call supported,-Walloca))
CFLAGS += -Walloca
endif
CFLAGS += -Wvla
CFLAGS += -Wpointer-arith
# Forbid old-style C function prototypes
# (skip for C++ files as older g++ complains about it)
ifeq (yes,$(call supported,-Wstrict-prototypes))
CFLAGS += $(if $(findstring .cpp,$(suffix $<)),,-Wstrict-prototypes)
endif

CFLAGS += -fvisibility=hidden

#
# Enable code sanitizers on demand and if supported by compiler
#

ifdef WITH_ASAN
CFLAGS += -DWITH_ASAN
ifeq (yes,$(call supported,-fsanitize=address))
SANITIZERS += -fsanitize=address
else
$(error -fsanitize=address requested but $(CC) does not seem to support it)
endif
endif

ifdef WITH_MSAN
CFLAGS += -DWITH_MSAN
ifeq (yes,$(call supported,-fsanitize=memory))
SANITIZERS += -fsanitize=memory -fsanitize-memory-track-origins=2
else
$(error -fsanitize=memory requested but $(CC) does not seem to support it)
endif
endif

ifdef WITH_TSAN
CFLAGS += -DWITH_TSAN
ifeq (yes,$(call supported,-fsanitize=thread))
SANITIZERS += -fsanitize=thread
else
$(error -fsanitize=thread requested but $(CC) does not seem to support it)
endif
endif

ifdef WITH_UBSAN
CFLAGS += -DWITH_UBSAN
ifeq (yes,$(call supported,-fsanitize=undefined))
SANITIZERS += -fsanitize=undefined
else
$(error -fsanitize=undefined requested but $(CC) does not seem to support it)
endif
ifeq (yes,$(call supported,-fsanitize=integer))
SANITIZERS += -fsanitize=integer
else
$(warning -fsanitize=integer not supported by $(CC), skipping...)
endif
ifeq (yes,$(call supported,-fsanitize=nullability))
SANITIZERS += -fsanitize=nullability
else
$(warning -fsanitize=nullability not supported by $(CC), skipping...)
endif
endif

ifeq (yes,$(WITH_FATAL_SANITIZERS))
SANITIZERS += -fno-sanitize-recover=all
endif

CFLAGS  += $(SANITIZERS)
LDFLAGS += $(SANITIZERS)

# Binary format compatibility with Themis 0.9.6 on x86_64 architecture.
# https://github.com/cossacklabs/themis/pull/279
# Themis 0.9.6 is going EOL on 2020-12-13 so it can be removed after that.
ifneq ($(WITH_SCELL_COMPAT),)
	CFLAGS += -DSCELL_COMPAT
endif

########################################################################

include src/soter/soter.mk
include src/themis/themis.mk
ifndef CARGO
include src/wrappers/themis/jsthemis/jsthemis.mk
include src/wrappers/themis/themispp/themispp.mk
include src/wrappers/themis/wasm/wasmthemis.mk
include jni/themis_jni.mk
include tests/test.mk
include tools/afl/fuzzy.mk
endif

########################################################################
#
# Principal Makefile targets
#

all: themis_static soter_static themis_shared soter_shared themis_pkgconfig soter_pkgconfig
	@echo $(VERSION)

soter_static:  $(BIN_PATH)/$(LIBSOTER_A)
soter_shared:  $(BIN_PATH)/$(LIBSOTER_SO)
themis_static: $(BIN_PATH)/$(LIBTHEMIS_A)
themis_shared: $(BIN_PATH)/$(LIBTHEMIS_SO)
themis_jni:    $(BIN_PATH)/$(LIBTHEMISJNI_SO)

soter_pkgconfig:  $(BIN_PATH)/libsoter.pc
themis_pkgconfig: $(BIN_PATH)/libthemis.pc

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

get_version:
	@echo $(VERSION)

for-audit: $(SOTER_AUD) $(THEMIS_AUD)

########################################################################
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

########################################################################
#
# Themis Core installation
#

# Red Hat systems usually do not have "lsb_release" in their default setup
# so we look into the release version files from "centos-release" package.
ifdef IS_LINUX
ifeq ($(shell . /etc/os-release; echo $$ID),centos)
IS_CENTOS := true
LD_SO_CONF = $(DESTDIR)/etc/ld.so.conf.d/themis.conf
endif
endif

install: all install_soter install_themis
	@echo -n "Themis installed to $(PREFIX)"
	@$(PRINT_OK_)
# CentOS does not have /usr/local/lib in the default search path, add it there.
ifeq ($(IS_CENTOS),true)
	-@mkdir -p "$$(dirname "$(LD_SO_CONF)")"
	-@echo "$(libdir)" > "$(LD_SO_CONF)" && echo "Added $(libdir) to $(LD_SO_CONF)"
endif
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

uninstall: uninstall_themis uninstall_soter
	@echo -n "Themis uninstalled from $(PREFIX) "
	@$(PRINT_OK_)
# Remove non-standard library search path created by "install" for CentOS.
ifeq ($(IS_CENTOS),true)
	@rm -f "$(LD_SO_CONF)"
endif

########################################################################
#
# Themis distribution tarball
#

DIST_DIR = themis_$(VERSION)

# Themis Core source code, tests, docs
DIST_FILES += docs src tests Makefile VERSION
DIST_FILES += README.md CHANGELOG.md LICENSE
DIST_FILES += PKGBUILD.MSYS2 Themis.nsi
# Supporting files for language wrappers
DIST_FILES += Cargo.toml
DIST_FILES += CMakeLists.txt
DIST_FILES += gothemis
DIST_FILES += jni
DIST_FILES += scripts tools

dist:
	@mkdir -p $(DIST_DIR)
	@rsync -a $(DIST_FILES) $(DIST_DIR)
	@tar czf $(DIST_DIR).tar.gz $(DIST_DIR)
	@rm -rf $(DIST_DIR)
	@echo $(DIST_DIR).tar.gz

unpack_dist:
	@tar -xf $(DIST_DIR).tar.gz

########################################################################
#
# Themis wrapper installation
#

## PHP #########################

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

ifneq ("$(wildcard src/wrappers/themis/php/Makefile)","")
PHP_THEMIS_INSTALL = 1
endif

phpthemis_uninstall: CMD = if [ -e src/wrappers/themis/php/Makefile ]; then cd src/wrappers/themis/php && make distclean ; fi;
phpthemis_uninstall:
ifdef PHP_THEMIS_INSTALL
	@echo -n "phpthemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: phpthemis_uninstall

## Ruby ########################

rbthemis_install: CMD = cd src/wrappers/themis/ruby && gem build rbthemis.gemspec && gem install ./*.gem $(_GEM_INSTALL_OPTIONS)
rbthemis_install:
ifdef RUBY_GEM_VERSION
	@echo -n "rbthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: ruby gem not found"
	@exit 1
endif

rbthemis_uninstall: CMD = gem uninstall themis
rbthemis_uninstall:
ifdef RUBY_GEM_VERSION
	@echo -n "rbthemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: rbthemis_uninstall

## Python ######################

ifdef PIP_VERSION
PIP_THEMIS_INSTALL := $(shell pip freeze |grep themis)
endif

pythemis_install: CMD = cd src/wrappers/themis/python/ && pip3 install .
pythemis_install:
ifeq ($(PYTHON3_VERSION),)
	@echo "python3 not found"
	@exit 1
endif
	@echo -n "pythemis install "
	@$(BUILD_CMD_)

########################################################################
#
# Packaging Themis Core: Linux distributions
#

ifeq ($(ENGINE),boringssl)
ifeq ($(CRYPTO_ENGINE_LIB_PATH),)
PACKAGE_EMBEDDED_BORINGSSL := yes
endif
endif

COSSACKLABS_URL = https://www.cossacklabs.com
MAINTAINER = "Cossack Labs Limited <dev@cossacklabs.com>"
LICENSE_NAME = "Apache License Version 2.0"

DEB_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEB_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
ifeq ($(PACKAGE_EMBEDDED_BORINGSSL),yes)
# fpm has "--provides" option, but it eats package versions when we need to
# preserve them for correct dependency resolution. Insert fields directly.
DEB_DEPENDENCIES += --deb-field "Provides: $(CANONICAL_PACKAGE_NAME) (= $(VERSION)+$(OS_CODENAME))"
DEB_DEPENDENCIES += --conflicts $(CANONICAL_PACKAGE_NAME)
DEB_DEPENDENCIES += --replaces  $(CANONICAL_PACKAGE_NAME)
DEB_DEPENDENCIES_DEV += --deb-field "Provides: $(DEB_CANONICAL_DEV_PACKAGE_NAME) (= $(VERSION)+$(OS_CODENAME))"
DEB_DEPENDENCIES_DEV += --conflicts $(DEB_CANONICAL_DEV_PACKAGE_NAME)
DEB_DEPENDENCIES_DEV += --replaces  $(DEB_CANONICAL_DEV_PACKAGE_NAME)
else
# If we were using native Debian packaging, dpkg-shlibdeps could supply us with
# accurate dependency information. However, we build packages manually, so we
# use dependencies of "libssl-dev" as a proxy. Typically this is "libssl1.1".
#
# Example output of "apt-cache depends" (from Ubuntu 16.04):
#
# libssl-dev
#   Depends: libssl1.0.0
#   Depends: zlib1g-dev
#   Recommends: libssl-doc
DEB_DEPENDENCIES += $(shell apt-cache depends libssl-dev | awk '$$1 == "Depends:" && $$2 ~ /^libssl/ { print "--depends", $$2 }' )
DEB_DEPENDENCIES_DEV += --depends libssl-dev
endif
DEB_DEPENDENCIES_DEV += --depends "$(PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)"
DEB_DEPENDENCIES_THEMISPP = --depends "$(DEB_CANONICAL_DEV_PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)"
DEB_DEPENDENCIES_JNI += --depends "$(CANONICAL_PACKAGE_NAME) >= $(VERSION)+$(OS_CODENAME)"

ifeq ($(PACKAGE_EMBEDDED_BORINGSSL),yes)
RPM_DEPENDENCIES += --provides  $(CANONICAL_PACKAGE_NAME)
RPM_DEPENDENCIES += --conflicts $(CANONICAL_PACKAGE_NAME)
RPM_DEPENDENCIES += --replaces  $(CANONICAL_PACKAGE_NAME)
RPM_DEPENDENCIES_DEV += --provides  $(RPM_CANONICAL_DEV_PACKAGE_NAME)
RPM_DEPENDENCIES_DEV += --conflicts $(RPM_CANONICAL_DEV_PACKAGE_NAME)
RPM_DEPENDENCIES_DEV += --replaces  $(RPM_CANONICAL_DEV_PACKAGE_NAME)
else
RPM_DEPENDENCIES     += --depends openssl-libs
RPM_DEPENDENCIES_DEV += --depends openssl-devel
endif
RPM_DEPENDENCIES_DEV += --depends "$(PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)"
RPM_DEPENDENCIES_THEMISPP = --depends "$(RPM_CANONICAL_DEV_PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)"
RPM_DEPENDENCIES_JNI += --depends "$(CANONICAL_PACKAGE_NAME) >= $(RPM_VERSION)-$(RPM_RELEASE_NUM)"
RPM_RELEASE_NUM = 1

OS_NAME := $(shell lsb_release -is 2>/dev/null || printf 'unknown')
ifeq ($(OS_NAME),$(filter $(OS_NAME),Debian Ubuntu))
#0.9.4-153-g9915004+jessie_amd64.deb.
	NAME_SUFFIX = $(VERSION)+$(DEB_CODENAME)_$(DEB_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
	DEB_LIBDIR := /lib/$(shell dpkg-architecture -qDEB_HOST_MULTIARCH)
else ifeq ($(OS_NAME),$(filter $(OS_NAME),RedHatEnterpriseServer CentOS))
	OS_NAME = $(shell cat /etc/os-release | grep -e "^ID=\".*\"" | cut -d'"' -f2)
	OS_VERSION = $(shell cat /etc/os-release | grep -i version_id|cut -d'"' -f2)
	ARCHITECTURE = $(shell arch)
	RPM_VERSION = $(shell echo -n "$(VERSION)"|sed s/-/_/g)
	NAME_SUFFIX = $(RPM_VERSION).$(OS_NAME)$(OS_VERSION).$(ARCHITECTURE).rpm
	RPM_LIBDIR := /$(shell [ $$(arch) == "x86_64" ] && echo "lib64" || echo "lib")
endif

CANONICAL_PACKAGE_NAME         = libthemis
DEB_CANONICAL_DEV_PACKAGE_NAME = $(CANONICAL_PACKAGE_NAME)-dev
RPM_CANONICAL_DEV_PACKAGE_NAME = $(CANONICAL_PACKAGE_NAME)-devel
ifeq ($(PACKAGE_EMBEDDED_BORINGSSL),yes)
PACKAGE_NAME = libthemis-boringssl
else
PACKAGE_NAME = libthemis
endif
DEB_DEV_PACKAGE_NAME = $(PACKAGE_NAME)-dev
RPM_DEV_PACKAGE_NAME = $(PACKAGE_NAME)-devel
DEB_THEMISPP_PACKAGE_NAME = libthemispp-dev
RPM_THEMISPP_PACKAGE_NAME = libthemispp-devel
JNI_PACKAGE_NAME = libthemis-jni

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
DEV_PACKAGE_FILES += $(libdir)/$(LIBSOTER_A)
DEV_PACKAGE_FILES += $(libdir)/$(LIBSOTER_LINK)
DEV_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_A)
DEV_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_LINK)

LIB_PACKAGE_FILES += $(libdir)/$(LIBSOTER_SO)
LIB_PACKAGE_FILES += $(libdir)/$(LIBTHEMIS_SO)

THEMISPP_PACKAGE_FILES += $(includedir)/themispp/

JNI_PACKAGE_FILES += $(jnidir)/$(LIBTHEMISJNI_SO)

deb: MODE_PACKAGING = 1
deb: DESTDIR = $(BIN_PATH)/deb/root
deb: PREFIX = /usr
deb: libdir = $(PREFIX)$(DEB_LIBDIR)
deb: jnidir = $(PREFIX)$(DEB_LIBDIR)/jni

deb: install themispp_install themis_jni_install
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@printf "ldconfig" > $(POST_UNINSTALL_SCRIPT)

	@find $(DESTDIR) -name '*.$(SHARED_EXT)*' -type f -exec strip -o {} {} \;

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

	@fpm --input-type dir \
		 --output-type deb \
		 --name $(JNI_PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/$(JNI_PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEB_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEB_DEPENDENCIES_JNI) \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --deb-priority optional \
		 --category $(PACKAGE_CATEGORY) \
		 $(foreach file,$(JNI_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

	@find $(BIN_PATH) -name \*.deb

rpm: MODE_PACKAGING = 1
rpm: DESTDIR = $(BIN_PATH)/rpm/root
rpm: PREFIX = /usr
rpm: libdir = $(PREFIX)$(RPM_LIBDIR)

rpm: install themispp_install themis_jni_install
	@printf "ldconfig" > $(POST_INSTALL_SCRIPT)
	@printf "ldconfig" > $(POST_UNINSTALL_SCRIPT)

	@find $(DESTDIR) -name '*.$(SHARED_EXT)*' -type f -exec strip -o {} {} \;

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

	@fpm --input-type dir \
         --output-type rpm \
         --name $(JNI_PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES_JNI) \
         --package $(BIN_PATH)/rpm/$(JNI_PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(foreach file,$(JNI_PACKAGE_FILES),$(DESTDIR)/$(file)=$(file))

	@find $(BIN_PATH) -name \*.rpm

########################################################################
#
# Packaging Themis Core: Windows (NSIS)
#

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

########################################################################
#
# Packaging PHP Themis: Linux distributions
#

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

PKGINFO_PATH = PKGINFO

pkginfo:
	@echo "$$PKGINFO" > $(PKGINFO_PATH)

PHP_VERSION_FULL:=$(shell php -r "echo PHP_MAJOR_VERSION.'.'.PHP_MINOR_VERSION;" 2>/dev/null)
ifeq ($(OS_CODENAME),jessie)
    PHP_DEPENDENCIES += --depends php5
else
    PHP_DEPENDENCIES += --depends php$(PHP_VERSION_FULL)
endif
PHP_DEPENDENCIES += --depends "$(CANONICAL_PACKAGE_NAME) >= $(VERSION)+$(OS_CODENAME)"

PHP_PACKAGE_NAME:=libphpthemis-php$(PHP_VERSION_FULL)
PHP_POST_INSTALL_SCRIPT:=./scripts/phpthemis_postinstall.sh
PHP_PRE_UNINSTALL_SCRIPT:=./scripts/phpthemis_preuninstall.sh
PHP_API:=$(shell php -i 2>/dev/null|grep 'PHP API'|sed 's/PHP API => //')
PHP_LIB_MAP:=./src/wrappers/themis/$(PHP_FOLDER)/.libs/phpthemis.so=/usr/lib/php/$(PHP_API)/

deb_php: MODE_PACKAGING = 1
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
		 $(PHP_DEPENDENCIES) \
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
