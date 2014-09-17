SOTER_OBJ = $(patsubst %.c,%.o, $(wildcard $(CRYPTO_ENGINE)/*.c))

# Put path to your OpenSSL/LibreSSL here
OPENSSL_DIR = /home/ignat/ph/librebin

CFLAGS += -I$(OPENSSL_DIR)/include
LDFLAGS += -L$(OPENSSL_DIR)/lib -lcrypto
 