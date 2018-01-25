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
TEST_OBJ_PATH = $(TEST_BIN_PATH)/obj

CFLAGS += -I$(SRC_PATH) -I$(SRC_PATH)/wrappers/themis/ -I/usr/local/include -fPIC
LDFLAGS += -L/usr/local/lib

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

AWK_CMD = awk '{ printf "%-30s %-10s\n",$$1, $$2; }'
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_OK_ = printf "$(OK_STRING)\n" | $(AWK_CMD)
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;


UNAME=$(shell uname)

ifeq ($(UNAME),Darwin)
	IS_MACOS := true
endif

define themisecho
      @tput setaf 6
      @echo $1
      @tput sgr0
endef

ifeq ($(ENGINE),)
	ENGINE=libressl
endif

#default engine
ifeq ($(PREFIX),)
PREFIX = /usr

# MacOS
ifdef IS_MACOS
	PREFIX = /usr/local
endif

endif

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

DEFAULT_VERSION := 0.9.5
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
GO_VERSION := $(shell go version 2>&1)
NPM_VERSION := $(shell npm --version 2>/dev/null)
PIP_VERSION := $(shell pip --version 2>/dev/null)
PYTHON_VERSION := $(shell python --version 2>&1)
PYTHON3_VERSION := $(shell python3 --version 2>/dev/null)
ifdef PIP_VERSION
PIP_THEMIS_INSTALL := $(shell pip freeze |grep themis)
endif
ifneq ("$(wildcard src/wrappers/themis/php/Makefile)","")
PHP_THEMIS_INSTALL = 1
endif

SHARED_EXT = so

IS_LINUX = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "linux")
IS_MINGW = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "mingw")
IS_CLANG_COMPILER = $(shell $(CC) --version 2>&1 | $(EGREP) -i -c "clang version")

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

ifdef COVERAGE
	CFLAGS += -g -O0 --coverage
	COVERLDFLAGS = --coverage
else
	COVERLDFLAGS =
endif

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

ifneq ($(GEM_INSTALL_OPTIONS),)
	_GEM_INSTALL_OPTIONS = $(GEM_INSTALL_OPTIONS)
endif

# Should pay attention to warnings (some may be critical for crypto-enabled code (ex. signed-unsigned mismatch)
CFLAGS += -Werror -Wno-switch

# strict checks for docs
#CFLAGS += -Wdocumentation -Wno-error=documentation

# fixing compatibility between x64 0.9.6 and x64 0.9.7
# https://github.com/cossacklabs/themis/pull/279
ifeq ($(NO_SCELL_COMPAT),)
	CFLAGS += -DSCELL_COMPAT
endif

ifndef ERROR
include src/soter/soter.mk
include src/themis/themis.mk
include jni/themis_jni.mk
endif

JSTHEMIS_PACKAGE_VERSION=$(shell cat src/wrappers/themis/jsthemis/package.json \
  | grep version \
  | head -1 \
  | awk -F: '{ print $$2 }' \
  | sed 's/[",]//g' \
  | tr -d '[[:space:]]')

all: err themis_static themis_shared
	@echo $(VERSION)

soter_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)

soter_static: $(SOTER_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

soter_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT) $(SOTER_OBJ) $(LDFLAGS) $(COVERLDFLAGS)

soter_shared: $(SOTER_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)
ifdef IS_MACOS
	@install_name_tool -id "$(PREFIX)/lib/lib$(SOTER_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)
	@install_name_tool -change "$(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)" "$(PREFIX)/lib/lib(SOTER_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(SOTER_BIN).$(SHARED_EXT)
endif

themis_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(THEMIS_BIN).a $(THEMIS_OBJ)

themis_static: soter_static $(THEMIS_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)

themis_shared: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT) $(THEMIS_OBJ) -L$(BIN_PATH) -l$(SOTER_BIN) $(COVERLDFLAGS)

themis_shared: soter_shared $(THEMIS_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)
ifdef IS_MACOS
	@install_name_tool -id "$(PREFIX)/lib/lib$(THEMIS_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)
	@install_name_tool -change "$(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)" "$(PREFIX)/lib/lib$(THEMIS_BIN).$(SHARED_EXT)" $(BIN_PATH)/lib$(THEMIS_BIN).$(SHARED_EXT)
endif

themis_jni: CMD = $(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_JNI_BIN).$(SHARED_EXT) $(THEMIS_JNI_OBJ) -L$(BIN_PATH) -l$(THEMIS_BIN) -l$(SOTER_BIN)

themis_jni: themis_static $(THEMIS_JNI_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


$(OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

#$(AUD_PATH)/%: CMD = $(CC) $(CFLAGS) -E -dI -dD $< -o $@
$(AUD_PATH)/%: CMD = ./scripts/pp.sh  $< $@

$(AUD_PATH)/%: $(SRC_PATH)/%
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -DNIST_STS_EXE_PATH=$(realpath $(NIST_STS_DIR)) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

$(TEST_OBJ_PATH)/%.opp: CMD = $(CXX) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@

$(TEST_OBJ_PATH)/%.opp: $(TEST_SRC_PATH)/%.cpp
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

include tests/test.mk

err: ; $(ERROR)

clean: CMD = rm -rf $(BIN_PATH)

clean: nist_rng_test_suite_clean
	@$(BUILD_CMD)

make_install_dirs: CMD = mkdir -p $(PREFIX)/include/themis $(PREFIX)/include/soter $(PREFIX)/lib

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

install: install_soter_headers install_themis_headers install_static_libs install_shared_libs

install_all: install themispp_install pythemis_install rubythemis_install phpthemis_install

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

rubythemis_uninstall: CMD = gem uninstall themis

rubythemis_uninstall:
ifdef RUBY_GEM_VERSION
	@echo -n "rubythemis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: CMD = rm -rf $(PREFIX)/include/themis && rm -rf $(PREFIX)/include/soter && rm -f $(PREFIX)/lib/libsoter.a && rm -f $(PREFIX)/lib/libthemis.a && rm -f $(PREFIX)/lib/libsoter.$(SHARED_EXT) && rm -f $(PREFIX)/lib/libthemis.$(SHARED_EXT)

uninstall: phpthemis_uninstall rubythemis_uninstall themispp_uninstall
	@echo -n "themis uninstall "
	@$(BUILD_CMD_)

ifeq ($(PHP_VERSION),5)
    PHP_FOLDER = php
else
    PHP_FOLDER = php7
endif

phpthemis_install: CMD = cd src/wrappers/themis/$(PHP_FOLDER) && phpize && ./configure && make install

phpthemis_install: install
ifdef PHP_VERSION
	@echo -n "phpthemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: php not found"
	@exit 1
endif

rubythemis_install: CMD = cd src/wrappers/themis/ruby && gem build rubythemis.gemspec && gem install ./*.gem $(_GEM_INSTALL_OPTIONS)

rubythemis_install: install
ifdef RUBY_GEM_VERSION
	@echo -n "rubythemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: ruby gem not found"
	@exit 1
endif

pythemis_install: CMD = cd src/wrappers/themis/python/ && python2 setup.py install --record files.txt

pythemis_install: install
ifdef PYTHON_VERSION
	@echo -n "pythemis install "
	@$(BUILD_CMD_)
else
	@echo "Error: python not found"
	@exit 1
endif
ifdef PYTHON3_VERSION
	@cd src/wrappers/themis/python/ && python3 setup.py install --record files3.txt
endif

themispp_install: CMD = install $(SRC_PATH)/wrappers/themis/themispp/*.hpp $(PREFIX)/include/themispp

themispp_install: install
	@mkdir -p $(PREFIX)/include/themispp
	@$(BUILD_CMD)

themispp_uninstall: CMD = rm -rf $(PREFIX)/include/themispp

themispp_uninstall:
	@echo -n "themispp uninstall "
	@$(BUILD_CMD_)

soter_collect_headers:
	@mkdir -p $(BIN_PATH)/include/soter
	@cd src/soter && find . -name \*.h -exec cp --parents {} ../../$(BIN_PATH)/include/soter/ \; && cd - 1 > /dev/null

themis_collect_headers:
	@mkdir -p $(BIN_PATH)/include/themis
	@cd src/themis && find . -name \*.h -exec cp --parents {} ../../$(BIN_PATH)/include/themis/ \; && cd - 1 > /dev/null

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

DEBIAN_CODENAME := $(shell lsb_release -cs 2> /dev/null)
DEBIAN_ARCHITECTURE = `dpkg --print-architecture 2>/dev/null`
DEBIAN_DEPENDENCIES := --depends openssl

RPM_DEPENDENCIES = --depends openssl
RPM_RELEASE_NUM = 1

ifeq ($(shell lsb_release -is 2> /dev/null),Debian)
#0.9.4-153-g9915004+jessie_amd64.deb.
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else ifeq ($(shell lsb_release -is 2> /dev/null),Ubuntu)
	NAME_SUFFIX = $(VERSION)+$(DEBIAN_CODENAME)_$(DEBIAN_ARCHITECTURE).deb
	OS_CODENAME = $(shell lsb_release -cs)
else
# centos/rpm
	OS_NAME = $(shell cat /etc/os-release | grep -e "^ID=\".*\"" | cut -d'"' -f2)
	OS_VERSION = $(shell cat /etc/os-release | grep -i version_id|cut -d'"' -f2)
	ARCHITECTURE = $(shell arch)
	RPM_VERSION = $(shell echo -n "$(VERSION)"|sed s/-/_/g)
	NAME_SUFFIX = $(RPM_VERSION).$(OS_NAME)$(OS_VERSION).$(ARCHITECTURE).rpm
endif

PACKAGE_NAME = themis
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

deb: test soter_static themis_static soter_shared themis_shared collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/deb

#libPACKAGE-dev
	@fpm --input-type dir \
		 --output-type deb \
		 --name lib$(PACKAGE_NAME)-dev \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/lib$(PACKAGE_NAME)-dev_$(NAME_SUFFIX) \
		 --architecture $(DEBIAN_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEBIAN_DEPENDENCIES) --depends "lib$(PACKAGE_NAME) = $(VERSION)+$(OS_CODENAME)" \
		 --deb-priority optional \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --category $(PACKAGE_CATEGORY) \
		 $(HEADER_FILES_MAP)

#libPACKAGE
	@fpm --input-type dir \
		 --output-type deb \
		 --name lib$(PACKAGE_NAME) \
		 --license $(LICENSE_NAME) \
		 --url '$(COSSACKLABS_URL)' \
		 --description '$(SHORT_DESCRIPTION)' \
		 --maintainer $(MAINTAINER) \
		 --package $(BIN_PATH)/deb/lib$(PACKAGE_NAME)_$(NAME_SUFFIX) \
		 --architecture $(DEBIAN_ARCHITECTURE) \
		 --version $(VERSION)+$(OS_CODENAME) \
		 $(DEBIAN_DEPENDENCIES) \
		 --after-install $(POST_INSTALL_SCRIPT) \
		 --after-remove $(POST_UNINSTALL_SCRIPT) \
		 --deb-priority optional \
		 --category $(PACKAGE_CATEGORY) \
		 $(BINARY_LIBRARY_MAP)

# it's just for printing .deb files
	@find $(BIN_PATH) -name \*.deb


rpm: test themis_static themis_shared soter_static soter_shared collect_headers install_shell_scripts strip symlink_realname_to_soname
	@mkdir -p $(BIN_PATH)/rpm
#libPACKAGE-devel
	@fpm --input-type dir \
         --output-type rpm \
         --name lib$(PACKAGE_NAME)-devel \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         $(RPM_DEPENDENCIES) --depends "lib$(PACKAGE_NAME) = $(RPM_VERSION)-$(RPM_RELEASE_NUM)" \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-devel-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
           $(HEADER_FILES_MAP)
#libPACKAGE
	@fpm --input-type dir \
         --output-type rpm \
         --name lib$(PACKAGE_NAME) \
         --license $(LICENSE_NAME) \
         --url '$(COSSACKLABS_URL)' \
         --description '$(SHORT_DESCRIPTION)' \
         --rpm-summary '$(RPM_SUMMARY)' \
         --maintainer $(MAINTAINER) \
         --after-install $(POST_INSTALL_SCRIPT) \
         --after-remove $(POST_UNINSTALL_SCRIPT) \
         $(RPM_DEPENDENCIES) \
         --package $(BIN_PATH)/rpm/lib$(PACKAGE_NAME)-$(NAME_SUFFIX) \
         --version $(RPM_VERSION) \
         --category $(PACKAGE_CATEGORY) \
         $(BINARY_LIBRARY_MAP)
# it's just for printing .rpm files
	@find $(BIN_PATH) -name \*.rpm
