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
if input_args.length != 4
  STDERR.puts "Usage: <command: enc | dec | sign | verify > <send_private_key> <recipient_public_key> <message>\n"
  exit 1
end

command = input_args[0]
private_key_path = input_args[1].dup
public_key_path = input_args[2].dup
message = input_args[3].dup
file = File.open(private_key_path, "r")
private_key = file.read
file.close

file = File.open(public_key_path, "r")
public_key = file.read
file.close

smessage = Themis::Smessage.new(private_key, public_key)
if command == "enc"
  encr_message = smessage.wrap(message)
  puts Base64.strict_encode64(encr_message)
elsif command == "dec"
  decr_message = smessage.unwrap(Base64.decode64(message))
  puts decr_message
elsif command == "sign"
  encr_message = Themis::s_sign(private_key, message)
  puts Base64.strict_encode64(encr_message)
elsif command == "verify"
  decr_message = Themis::s_verify(public_key, Base64.decode64(message))
  puts decr_message
else
  STDERR.puts "Wrong command, use <enc | dev | sign | verify>"
  exit 1
end
