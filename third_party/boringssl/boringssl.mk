#
# Copyright (c) 2020 Cossack Labs Limited
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

## Update BoringSSL submodule and associated intermediate files.
update_boringssl:
	@git submodule update --init
	@cd third_party/boringssl/src && \
	 git checkout master && git pull
ifdef IS_MACOS
	@third_party/boringssl/generate_build_files.sh
	@third_party/boringssl/update_xcodeproj.rb
else
	$(warning Xcode project files are not updated.)
	$(warning Run "make $(MAKECMDGOALS)" on macOS.)
endif
