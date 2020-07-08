#!/usr/bin/env ruby
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

require 'rbthemis'
require 'base64'

if ARGV.length < 3 || ARGV.length > 4
  STDERR.puts "usage: {enc|dec} <passphrase> <message> [context]"
  exit 1
end

command    = ARGV[0]
passphrase = ARGV[1]
message    = ARGV[2]
context    = ARGV.length == 4 ? ARGV[3] : nil

scell = Themis::ScellSealPassphrase.new(passphrase)

case command
when 'enc'
  encrypted = scell.encrypt(message, context)
  puts Base64.strict_encode64(encrypted)
when 'dec'
  decrypted = scell.decrypt(Base64.decode64(message), context)
  puts decrypted
else
  STDERR.puts "unknown command \"#{command}\", use \"enc\" or \"dec\""
  exit 1
end
