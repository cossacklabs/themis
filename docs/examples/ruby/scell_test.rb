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

#!/usr/bin/env ruby

require 'rubygems'
require 'rubythemis'

key = 'password'
context = 'context'
message = 'test message'

scell_full = Themis::Scell.new(key, Themis::Scell::SEAL_MODE)
mm = scell_full.encrypt(message, context)
p mm
p scell_full.decrypt(mm, context)

mm = scell_full.encrypt(message)
p mm
p scell_full.decrypt(mm)

scell_auto_split = Themis::Scell.new(key, Themis::Scell::TOKEN_PROTECT_MODE)

mm, ss = scell_auto_split.encrypt(message, context)
p mm, ss
p scell_auto_split.decrypt([mm, ss], context)

mm = scell_auto_split.encrypt(message)
p mm
p scell_auto_split.decrypt(mm)

scell_user_split = Themis::Scell.new(key, Themis::Scell::CONTEXT_IMPRINT_MODE)
mm = scell_user_split.encrypt(message, context)
p mm
p scell_user_split.decrypt(mm, context)

# mm = scell_user_split.encrypt(message)
# p mm
# p scell_user_split.decrypt(mm)
