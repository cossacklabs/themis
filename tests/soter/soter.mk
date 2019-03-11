#
# Copyright (c) 2015 Cossack Labs Limited
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

SOTER_TEST_SOURCES = $(wildcard tests/soter/*.c)
SOTER_TEST_HEADERS = $(wildcard tests/soter/*.h)

SOTER_TEST_SRC = $(wildcard tests/soter/*.c)
SOTER_TEST_OBJ = $(patsubst $(TEST_SRC_PATH)/%.c,$(TEST_OBJ_PATH)/%.o, $(SOTER_TEST_SRC))

SOTER_TEST_FMT_SRC = $(SOTER_TEST_SOURCES) $(SOTER_TEST_HEADERS)
SOTER_TEST_FMT_FIXUP = $(patsubst $(TEST_SRC_PATH)/%,$(TEST_OBJ_PATH)/%.fmt_fixup, $(SOTER_TEST_FMT_SRC))
SOTER_TEST_FMT_CHECK = $(patsubst $(TEST_SRC_PATH)/%,$(TEST_OBJ_PATH)/%.fmt_check, $(SOTER_TEST_FMT_SRC))
