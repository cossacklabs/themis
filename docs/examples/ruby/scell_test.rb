#!/usr/bin/env ruby
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

require 'rbthemis'
require 'base64'

passphrase = 'open sesame'
master_key = Themis::gen_sym_key
context = 'context'
message = 'test message'

puts "Secure Cell - Seal mode (master key)"
scell = Themis::ScellSeal.new(master_key)
encrypted = scell.encrypt(message, context)
decrypted = scell.decrypt(encrypted, context)
puts "Encoded:   #{Base64.encode64 message}"
puts "Encrypted: #{Base64.encode64 encrypted}"
puts "Decrypted: #{decrypted}"
puts

puts "Secure Cell - Seal mode (passphrase)"
scell = Themis::ScellSealPassphrase.new(passphrase)
encrypted = scell.encrypt(message, context)
decrypted = scell.decrypt(encrypted, context)
puts "Encoded:   #{Base64.encode64 message}"
puts "Encrypted: #{Base64.encode64 encrypted}"
puts "Decrypted: #{decrypted}"
puts

puts "Secure Cell - Token Protect mode"
scell = Themis::ScellTokenProtect.new(master_key)
encrypted, token = scell.encrypt(message, context)
decrypted = scell.decrypt(encrypted, token, context)
puts "Encoded:   #{Base64.encode64 message}"
puts "Encrypted: #{Base64.encode64 encrypted}"
puts "Token:     #{Base64.encode64 token}"
puts "Decrypted: #{decrypted}"
puts

puts "Secure Cell - Context Imprint mode"
scell = Themis::ScellContextImprint.new(master_key)
encrypted = scell.encrypt(message, context)
decrypted = scell.decrypt(encrypted, context)
puts "Encoded:   #{Base64.encode64 message}"
puts "Encrypted: #{Base64.encode64 encrypted}"
puts "Decrypted: #{decrypted}"
puts
