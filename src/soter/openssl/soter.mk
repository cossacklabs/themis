SOTER_SRC += $(wildcard $(CRYPTO_ENGINE)/*.c)

# Put path to your OpenSSL/LibreSSL here
OPENSSL_DIR = libs/librebin

ifneq ($(CRYPTO_ENGINE_INCLUDE_PATH),)
	CFLAGS += -I$(CRYPTO_ENGINE_INCLUDE_PATH)
endif
ifneq ($(CRYPTO_ENGINE_LIB_PATH),)
	LDFLAGS += -L$(CRYPTO_ENGINE_LIB_PATH)
endif
LDFLAGS += -lcrypto