#
# Copyright (c) 2017 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CC = clang

SRC_PATH = .
OBJ_PATH = .
BIN_PATH = .
CRYPTER_BIN = soter_openssl_crypter
CRYPTER_SRC = $(wildcard $(SRC_PATH)/*.c)
CRYPTER_OBJ = $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o, $(CRYPTER_SRC))

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
ERROR_COLOR=\033[31;01m
WARN_COLOR=\033[33;01m

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)

AWK_CMD = awk '{ printf "%-30s %-10s\n",$$1, $$2; }'
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_OK_ = printf "$(OK_STRING)\n" | $(AWK_CMD)
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_ERROR_ = printf "$(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_WARNING_ = printf "$(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;
BUILD_CMD_ = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR_); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING_); else $(PRINT_OK_); fi;


define themisecho
      @tput setaf 6
      @echo $1
      @tput sgr0
endef

CFLAGS = -I../../../ -I../

all: crypter_static

clean:
	rm -r $(BIN_PATH)/*.o
	rm -r $(BIN_PATH)/*.a

$(OBJ_PATH)/%.o: CMD = $(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(@D)
	@echo -n "compile "
	@$(BUILD_CMD)

crypter_static: CMD = $(AR) rcs $(BIN_PATH)/lib$(CRYPTER_BIN).a $(CRYPTER_OBJ)

crypter_static: $(CRYPTER_OBJ)
	@echo -n "link "
	@$(BUILD_CMD)


