THEMIS_TEST_SRC = $(wildcard tests/themis/*.c)
THEMIS_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(THEMIS_TEST_SRC))
