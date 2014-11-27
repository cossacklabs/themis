SPLINT_DIR = tests/tools/splint
SPLINT_SRC_DIR = $(SPLINT_DIR)/source
SPLINT_BIN_DIR = $(SPLINT_DIR)/splbin
SPLINT_EXE = $(SPLINT_BIN_DIR)/bin/splint
SPLINT_OUT_FILE = static_analysis.txt

build_splint:
	mkdir -p $(SPLINT_BIN_DIR)
	cd $(SPLINT_SRC_DIR) && ./configure --prefix=$(realpath $(SPLINT_BIN_DIR)) && make && make install
	-rm $(SPLINT_OUT_FILE)

SPLINT_FILES = $(patsubst %.c,%.splint, $(SOTER_SRC))

%.splint: %.c
	-$(SPLINT_EXE) $< -I$(SRC_PATH) -I$(OPENSSL_DIR)/include -D$(CRYPTO_ENGINE_DEF) +posixlib >> $(SPLINT_OUT_FILE)

static_analysis: build_splint $(SPLINT_FILES)
