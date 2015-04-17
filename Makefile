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

ifneq ($(ENGINE),)
ifeq ($(ENGINE),openssl)
	CRYPTO_ENGINE_DEF = OPENSSL	
	CRYPTO_ENGINE = $(SRC_PATH)/soter/openssl
	CFLAGS += -D$(CRYPTO_ENGINE_DEF)
else ifeq ($(ENGINE),libressl)
	CRYPTO_ENGINE_DEF = LIBRESSL	
	CRYPTO_ENGINE = $(SRC_PATH)/soter/openssl
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
IS_LINUX = $(shell $(CXX) -dumpmachine 2>&1 | $(EGREP) -c "linux")
IS_MINGW = $(shell $(CXX) -dumpmachine 2>&1 | $(EGREP) -c "mingw")
CLANG_COMPILER = $(shell $(CXX) --version 2>&1 | $(EGREP) -i -c "clang version")

ifeq ($(UNAME),Darwin)
AR = libtool
ARFLAGS = -static -o
CXX = c++
IS_GCC2 = $(shell $(CXX) -v 2>&1 | $(EGREP) -c gcc-932)
ifeq ($(IS_GCC2),1)
CXXFLAGS += -fno-coalesce-templates -fno-coalesce-static-vtables
LDLIBS += -lstdc++
LDFLAGS += -flat_namespace -undefined suppress -m
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
