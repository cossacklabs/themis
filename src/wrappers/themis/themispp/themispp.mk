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

THEMISPP_HEADERS += $(wildcard $(SRC_PATH)/wrappers/themis/themispp/*.hpp)
THEMISPP_HEADERS += $(wildcard $(SRC_PATH)/wrappers/themis/themispp/impl/*.hpp)

FMT_FIXUP += $(patsubst %,$(OBJ_PATH)/%.fmt_fixup, $(THEMISPP_HEADERS))
FMT_CHECK += $(patsubst %,$(OBJ_PATH)/%.fmt_check, $(THEMISPP_HEADERS))

themispp_install:
	@echo -n "install ThemisPP "
	@mkdir -p $(DESTDIR)/$(includedir)/themispp
	@mkdir -p $(DESTDIR)/$(includedir)/themispp/impl
	@$(INSTALL_DATA) $(SRC_PATH)/wrappers/themis/themispp/*.hpp      $(DESTDIR)/$(includedir)/themispp
	@$(INSTALL_DATA) $(SRC_PATH)/wrappers/themis/themispp/impl/*.hpp $(DESTDIR)/$(includedir)/themispp/impl
	@$(PRINT_OK_)

themispp_uninstall:
	@echo -n "uninstall ThemisPP "
	@rm -rf $(DESTDIR)/$(includedir)/themispp
	@$(PRINT_OK_)

uninstall: themispp_uninstall
