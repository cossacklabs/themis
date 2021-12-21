#!/usr/bin/env bash
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

set -euo pipefail

CONFCONT="extension=phpthemis.so"
SEARCH="Scan this dir for additional .ini files => "
PHPCONFD=`php -i | grep "$SEARCH" | sed "s/$SEARCH//"`;
if [ -d $PHPCONFD ]; then
    rm -f $PHPCONFD/20-phpthemis.ini;
fi
