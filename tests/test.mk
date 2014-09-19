include tests/soter/soter.mk


soter_test: $(SOTER_TEST_OBJ) static
	$(CC) -o $(TEST_BIN_PATH)/soter_test $(SOTER_TEST_OBJ) -L$(BIN_PATH) -lsoter $(LDFLAGS)

test:	soter_test