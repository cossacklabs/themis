#CC = clang
SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

CFLAGS += -I$(SRC_PATH) -fPIC 

ifeq ($(ENGINE),)
	ENGINE=libressl
endif

ifeq ($(PREFIX),)
PREFIX = /usr
endif

ifneq ($(ENGINE),)
ifeq ($(ENGINE),openssl)
	CRYPTO_ENGINE_DEF = OPENSSL
	CRYPTO_ENGINE_PATH=openssl
	CRYPTO_ENGINE = $(SRC_PATH)/soter/$(CRYPTO_ENGINE_PATH)
	CFLAGS += -D$(CRYPTO_ENGINE_DEF)
else ifeq ($(ENGINE),libressl)
	CRYPTO_ENGINE_DEF = LIBRESSL	
	CRYPTO_ENGINE_PATH=openssl
	CRYPTO_ENGINE = $(SRC_PATH)/soter/$(CRYPTO_ENGINE_PATH)
	CFLAGS += -D$(CRYPTO_ENGINE_DEF)
else
	ERROR = $(error error: engine $(ENGINE) unsupported...)
endif
endif

ifneq ($(ENGINE_INCLUDE_PATH),)
	CRYPTO_ENGINE_INCLUDE_PATH = $(ENGINE_INCLUDE_PATH)
endif
ifneq ($(ENGINE_LIB_PATH),)
	CRYPTO_ENGINE_LIB_PATH = $(ENGINE_LIB_PATH)
endif

UNAME=$(shell uname)
IS_LINUX = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "linux")
IS_MINGW = $(shell $(CC) -dumpmachine 2>&1 | $(EGREP) -c "mingw")
IS_CLANG_COMPILER = $(shell $(CC) --version 2>&1 | $(EGREP) -i -c "clang version")

ifeq ($(shell uname),Darwin)
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
CFLAFS += -isysroot $(ARCH)
endif
endif

ifdef DEBUG
# Making debug build for now
	CFLAGS += -DDEBUG -g
endif

# Should pay attention to warnings (some may be critical for crypto-enabled code (ex. signed-unsigned mismatch)
CFLAGS += -Werror -Wno-switch

ifndef ERROR
include src/soter/soter.mk
include src/themis/themis.mk
endif


all: err themis_static themis_shared

test_all: err test

soter_static: $(SOTER_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)

soter_shared: $(SOTER_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).so $(SOTER_OBJ) $(LDFLAGS)

themis_static: soter_static $(THEMIS_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(THEMIS_BIN).a $(THEMIS_OBJ)

themis_shared: soter_shared $(THEMIS_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_BIN).so $(THEMIS_OBJ) -L$(BIN_PATH) -l$(SOTER_BIN)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -DNIST_STS_EXE_PATH=$(realpath $(NIST_STS_DIR)) -I$(TEST_SRC_PATH) -c $< -o $@

include tests/test.mk

err: ; $(ERROR)

clean: nist_rng_test_suite
	rm -rf $(BIN_PATH)

install: err all
	mkdir -p $(PREFIX)/include/themis $(PREFIX)/include/soter $(PREFIX)/lib
	install $(SRC_PATH)/soter/*.h $(PREFIX)/include/soter
	install $(SRC_PATH)/themis/*.h $(PREFIX)/include/themis
	install $(BIN_PATH)/*.a $(PREFIX)/lib
	install $(BIN_PATH)/*.so $(PREFIX)/lib

uninstall:
	rm -rf $(PREFIX)/include/themis
	rm -rf $(PREFIX)/include/soter
	rm $(PREFIX)/lib/libsoter.a
	rm $(PREFIX)/lib/libthemis.a
	rm $(PREFIX)/lib/libsoter.so
	rm $(PREFIX)/lib/libthemis.so
