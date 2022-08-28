#
# Copyright (c) 2019 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

WASM_PATH = src/wrappers/themis/wasm

WASM_SRC += $(WASM_PATH)/package.json
WASM_SRC += $(wildcard $(WASM_PATH)/src/*.js)

WASM_RUNTIME = $(abspath $(WASM_PATH)/emscripten/runtime_exports.json)
WASM_PRE_JS  = $(abspath $(WASM_PATH)/emscripten/pre.js)

WASM_PACKAGE = $(BIN_PATH)/wasm-themis.tgz

$(BIN_PATH)/libthemis.js: LDFLAGS += -s EXPORTED_RUNTIME_METHODS=@$(WASM_RUNTIME)
$(BIN_PATH)/libthemis.js: LDFLAGS += -s ALLOW_TABLE_GROWTH
$(BIN_PATH)/libthemis.js: LDFLAGS += -s MODULARIZE=1
$(BIN_PATH)/libthemis.js: LDFLAGS += -s ALLOW_MEMORY_GROWTH=1
# FIXME(ilammy, 2020-11-29): rely in EMSCRIPTEN_KEEPALIVE instead of LINKABLE
# For some reason existing EMSCRIPTEN_KEEPALIVE macros do not work and without
# LINKABLE flag wasm-ld ends up stripping *all* Themis functions from "*.wasm"
# output, as if removed by dead code elimination.
$(BIN_PATH)/libthemis.js: LDFLAGS += -s LINKABLE=1
# FIXME(ilammy, 2021-11-24): figure out why Emscripten linker is so stupid
# Same as above. For some reason Emscripten linker will either strip everything
# from our WebAssembly module, or complain that BoringSSL functions that Themis
# does not use are missing the from the binary. Suppress the warnings.
$(BIN_PATH)/libthemis.js: LDFLAGS += -s ERROR_ON_UNDEFINED_SYMBOLS=0
$(BIN_PATH)/libthemis.js: LDFLAGS += --pre-js $(WASM_PRE_JS)
$(BIN_PATH)/libthemis.js: CMD = $(CC) -o $@ $(filter %.o %a, $^) -O3 $(LDFLAGS)
$(BIN_PATH)/libthemis.js: $(THEMIS_STATIC) $(WASM_RUNTIME) $(WASM_PRE_JS)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

$(WASM_PACKAGE): $(BIN_PATH)/libthemis.js $(WASM_SRC)
	@mkdir -p $(@D)
	@echo -n "pack $@ "
	@cp $(BIN_PATH)/libthemis.{js,wasm} $(WASM_PATH)/src
	@cd $(WASM_PATH) && npm install
	@cd $(WASM_PATH) && npm pack
	@mv $(WASM_PATH)/wasm-themis-*.tgz $(WASM_PACKAGE)
	@$(PRINT_OK_)

wasmthemis: check_emscripten $(WASM_PACKAGE)

check_emscripten:
ifndef IS_EMSCRIPTEN
	@printf "check Emscripten environment $(ERROR_STRING)\n"
	@echo ; \
	 echo "You seem to be trying to compile Themis for WebAssembly"; \
	 echo "without enabling Emscripten cross-compilation."; \
	 echo ; \
	 echo "When compiling Themis for WebAssembly you need to use"; \
	 echo "Emscripten helper tool 'emmake' for building:"; \
	 echo ; \
	 echo "    emmake $(MAKE) $(MAKECMDGOALS) $(MAKEFLAGS)"; \
	 echo ; \
	 if ! which emmake >/dev/null; then \
	     echo "You can activate Emscripten environment in your current shell"; \
	     echo "by sourcing environment configuration file of Emscripten SDK."; \
	     echo "It is usually located in your home directory:"; \
	     echo ; \
	     echo "    source ~/emsdk/emsdk_env.sh"; \
	     echo ; \
	 fi; \
	 if [ -d $(OBJ_PATH) ]; then \
	     echo "The build directory seems to contain previous build tree."; \
	     echo "Please clean the build directory by running"; \
	     echo ; \
	     echo "    make clean"; \
	     echo ; \
	     echo "and try again with 'emmake' in Emscripten environment."; \
	     echo ; \
	 fi
	@exit 1
endif
#   Typical version string:
#	emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 2.0.13 (681cdf3e7edeef79855adc2f2a04a1a2a44ec24f)
	@actual="$$(emcc --version | head -1 | sed -E 's/^emcc \([^()]*\) (.*) \([0-9a-f]*\)$$/\1/')"; \
     expected="$$(cat $(WASM_PATH)/emscripten/VERSION)"; \
	 if [[ "$$actual" != "$$expected" ]]; then \
	     echo "Current Emscripten environment is not supported!" ; \
	     echo "    actual:   $$actual" ; \
	     echo "    expected: $$expected" ; \
	     exit 1 ; \
	 fi

wasmthemis_install: CMD = npm install $(abspath $(WASM_PACKAGE))
wasmthemis_install:
ifdef NPM_VERSION
	@echo -n "wasm-themis install "
	@$(BUILD_CMD_)
else
	@echo "Error: npm not found"
	@exit 1
endif

wasmthemis_uninstall: CMD = rm -f $(WASM_PACKAGE) && npm uninstall wasm-themis
wasmthemis_uninstall:
ifdef NPM_VERSION
	@echo -n "wasm-themis uninstall "
	@$(BUILD_CMD_)
endif

uninstall: wasmthemis_uninstall
