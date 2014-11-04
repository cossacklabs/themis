COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

NIST_STS_DIR = tests/soter/nist-sts

include tests/soter/soter.mk

nist_rng_test_suite:
	mkdir -p $(NIST_STS_DIR)/obj
	$(MAKE) -C $(NIST_STS_DIR) $(MAKECMDGOALS)

soter_test: $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) static
	$(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS)

test:	soter_test nist_rng_test_suite