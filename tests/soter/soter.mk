SOTER_TEST_SRC = $(wildcard tests/soter/*.c)
SOTER_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(SOTER_TEST_SRC))
