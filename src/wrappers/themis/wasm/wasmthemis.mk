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

WASM_RUNTIME = $(abspath $(WASM_PATH)/runtime_exports.json)

$(BIN_PATH)/libthemis.js: LDFLAGS += -s EXTRA_EXPORTED_RUNTIME_METHODS=@$(WASM_RUNTIME)

$(BIN_PATH)/libthemis.js: CMD = $(CC) -o $@ $(filter %.o %a, $^) $(LDFLAGS)

$(BIN_PATH)/libthemis.js: $(THEMIS_STATIC) $(WASM_RUNTIME)
	@mkdir -p $(@D)
	@echo -n "link "
	@$(BUILD_CMD)

wasm_themis: check_emscripten $(BIN_PATH)/libthemis.js

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
