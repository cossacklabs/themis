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

require 'fileutils'
require 'rubygems'
require 'rbthemis'

if ARGV.length != 0 && ARGV.length != 2
    puts "Usage: ruby keygen.rb <private_key_path> <public_key_path>"
    exit 1
end

generator = Themis::SKeyPairGen.new

if ARGV.length == 0
    private_key_path = 'key'
    public_key_path = 'key.pub'
elsif ARGV.length == 2
    private_key_path = ARGV[0]
    public_key_path = ARGV[1]
end
keypair = generator.ec

File.open(private_key_path, 'w') { |file|
    file.write(keypair[0])
}

FileUtils.chmod 0400, private_key_path

File.open(public_key_path, 'w') { |file|
    file.write(keypair[1])
}
