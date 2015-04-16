COMMON_TEST_SRC = $(wildcard tests/common/*.c)
COMMON_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(COMMON_TEST_SRC))

NIST_STS_DIR = tests/soter/nist-sts

include tests/soter/soter.mk
include tests/tools/tools.mk
include tests/themis/themis.mk

nist_rng_test_suite:
	mkdir -p $(NIST_STS_DIR)/obj
	cd $(NIST_STS_DIR)/experiments && ./create-dir-script
	$(MAKE) -C $(NIST_STS_DIR)

soter_test: nist_rng_test_suite soter_static $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ)
	$(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS)
	
themis_test: themis_static $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ)
	$(CC) -o $(TEST_BIN_PATH)/themis_test $(THEMIS_TEST_OBJ) $(COMMON_TEST_OBJ) -L$(BIN_PATH) -lthemis -lsoter $(LDFLAGS)

test: soter_test themis_test