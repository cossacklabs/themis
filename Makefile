# We will use OpenSSL/LibreSSL for now
CRYPTO_ENGINE = src/soter/openssl
CFLAGS += -Isrc/soter -fPIC

BIN_PATH = build
OBJ_PATH = build/obj
TEST_PATH = build/tests

include src/soter/soter.mk

all: static

static: $(SOTER_OBJ)
	$(AR) rcs $(BIN_PATH)/lib$(SOTER_BIN).a $(SOTER_OBJ)
	
shared: $(SOTER_OBJ)
	$(CC) -shared -o $(BIN_PATH)/lib$(SOTER_BIN).so $(SOTER_OBJ)
	
include tests/test.mk
	
clean:
	rm -f $(SOTER_OBJ)
	rm -f $(TEST_OBJ)
	rm -f $(TEST_BIN)
	rm -f lib$(SOTER_BIN).a
	rm -f lib$(SOTER_BIN).so
