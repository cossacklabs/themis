#CC= clang
SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

# We will use OpenSSL/LibreSSL for now
CRYPTO_ENGINE_DEF = OPENSSL
CRYPTO_ENGINE = $(SRC_PATH)/soter/openssl
CFLAGS += -I$(SRC_PATH) -fPIC -D$(CRYPTO_ENGINE_DEF)

# Should pay attention to warnings (some may be critical for crypto-enabled code (ex. signed-unsigned mismatch)
CFLAGS += -Werror

# Making debug build for now
CFLAGS += -g

include src/soter/soter.mk
include src/themis/themis.mk

all: test

soter_static: $(SOTER_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)
	
soter_shared: $(SOTER_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).so $(SOTER_OBJ)
	
themis_static: soter_static $(THEMIS_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(THEMIS_BIN).a $(THEMIS_OBJ)
	
themis_shared: soter_shared $(THEMIS_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(THEMIS_BIN).so $(THEMIS_OBJ)
	
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -DNIST_STS_EXE_PATH=$(realpath $(NIST_STS_DIR)) -I$(TEST_SRC_PATH) -c $< -o $@

include tests/test.mk
	
clean: nist_rng_test_suite
	rm -rf $(BIN_PATH)
