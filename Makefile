SRC_PATH = src
BIN_PATH = build
OBJ_PATH = build/obj
TEST_SRC_PATH = tests
TEST_OBJ_PATH = build/tests/obj
TEST_BIN_PATH = build/tests

# We will use OpenSSL/LibreSSL for now
CRYPTO_ENGINE = $(SRC_PATH)/soter/openssl
CFLAGS += -I$(SRC_PATH) -fPIC

# Should pay attention to warnings (some may be critical for crypto-enabled code (ex. signed-unsigned mismatch)
CFLAGS += -Werror

# Making debug build for now
CFLAGS += -g

include src/soter/soter.mk

all: test

static: $(SOTER_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)
	
shared: $(SOTER_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).so $(SOTER_OBJ)
	
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(TEST_SRC_PATH) -c $< -o $@
	
include tests/test.mk
	
clean:
	rm -rf $(BIN_PATH)
