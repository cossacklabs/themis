# We will use OpenSSL/LibreSSL for now
CRYPTO_ENGINE = src/soter/openssl
CFLAGS += -Isrc/soter -fPIC
include $(CRYPTO_ENGINE)/soter.mk

SOTER_BIN = soter

TEST_OBJ += tests/soter/soter.o
TEST_BIN = test

all: static

static: $(SOTER_OBJ)
	$(AR) rcs lib$(SOTER_BIN).a $(SOTER_OBJ)
	
shared: $(SOTER_OBJ)
	$(CC) -shared -o lib$(SOTER_BIN).so $(SOTER_OBJ)
	
test: $(TEST_OBJ)
	$(CC) -o test $(TEST_OBJ) -L. -lsoter $(LDFLAGS)
	
clean:
	rm -f $(SOTER_OBJ)
	rm -f $(TEST_OBJ)
	rm -f $(TEST_BIN)
	rm -f lib$(SOTER_BIN).a
	rm -f lib$(SOTER_BIN).so
