SOTER_SRC = $(wildcard $(CRYPTO_ENGINE)/*.c)
SOTER_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(SOTER_SRC))

# Put path to your OpenSSL/LibreSSL here
OPENSSL_DIR = libs/librebin

CFLAGS += -I$(OPENSSL_DIR)/include
LDFLAGS += -L$(OPENSSL_DIR)/lib -lcrypto
