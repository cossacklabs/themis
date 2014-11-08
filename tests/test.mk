COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

NIST_STS_DIR = tests/soter/nist-sts

include tests/soter/soter.mk

nist_rng_test_suite:
	mkdir -p $(NIST_STS_DIR)/obj
	cd $(NIST_STS_DIR)/experiments && ./create-dir-script
ifeq (all,$(MAKECMDGOALS))
	# NIST makefile does not support "all" target, so make default
	$(MAKE) -C $(NIST_STS_DIR)
else
	$(MAKE) -C $(NIST_STS_DIR) $(MAKECMDGOALS)
endif

soter_test: nist_rng_test_suite static $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) static
	$(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS)

test: soter_test