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

#!/usr/bin/env ruby

require 'rubygems'
require 'rubythemis'
require 'base64'

input_args = ARGV

if input_args.length < 3 || input_args.length > 4 
	STDERR.puts "Usage: <command: enc | dec > <key> <message> <context (optional)>\n"
	exit 1
end

command = input_args[0]
key = input_args[1].dup
message = input_args[2].dup
context = nil

if input_args.length == 4
	context = input_args[3].dup
end

scell = Themis::Scell.new(key, Themis::Scell::SEAL_MODE)

if command == "enc"
	encr_message = scell.encrypt(message, context)
	puts Base64.strict_encode64(encr_message)
elsif command == "dec"
	decr_message = scell.decrypt(Base64.decode64(message), context)
	puts decr_message
else
	STDERR.puts "Wrong command, use \"enc\" or \"dec\""
	exit 1
end
