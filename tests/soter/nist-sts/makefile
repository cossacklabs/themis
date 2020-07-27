#
# Copyright (c) 2019 Cossack Labs Limited
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

CC ?= gcc

CFLAGS  += -Wall -Wno-unused
LDFLAGS += -lm

ifeq (yes,$(shell $(CC) --version | grep -qi clang && echo yes))
CFLAGS += -Wno-absolute-value
endif

ifeq (yes,$(shell $(CC) --version | grep -qi gcc && echo yes))
CFLAGS += -Wno-misleading-indentation
endif

SRCDIR = src
OBJDIR = obj

TARGET = assess

SRC = $(wildcard $(SRCDIR)/*.c)

OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEP = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.d)

all: $(TARGET)

-include $(DEP)

$(TARGET): $(OBJ)
	@echo " LD $@"
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo " CC $<"
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS) -MMD

clean:
	@rm -f $(TARGET) $(OBJ)

rebuild: clean $(TARGET)

.PHONY: all clean rebuild
