THEMIS_SRC = $(wildcard $(SRC_PATH)/themis/*.c)

THEMIS_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(THEMIS_SRC))

THEMIS_BIN = themis
