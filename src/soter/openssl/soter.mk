SOTER_SRC += $(wildcard $(CRYPTO_ENGINE)/*.c)

# Put path to your OpenSSL/LibreSSL here
OPENSSL_INCLUDE_DIR = libs/librebin/include
OPENSSL_LIB_DIR = libs/librebin/lib/$(SDK)-$(ARCH)

CFLAGS += -I$(OPENSSL_INCLUDE_DIR)
LDFLAGS += -L$(OPENSSL_LIB_DIR) -lcrypto
