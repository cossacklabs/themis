SOTER_SRC = $(wildcard $(SRC_PATH)/soter/*.c)

include $(CRYPTO_ENGINE)/soter.mk

SOTER_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SOTER_SRC))

SOTER_BIN = soter