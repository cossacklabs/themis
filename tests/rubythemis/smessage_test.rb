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
require 'test/unit'

class TestMessage < Test::Unit::TestCase
  def setup
    @keys = [
      [
        @ec256_priv = "\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf",
        @ec256_pub = "\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"
      ],
      [
        @rsa1024_priv = "\x52\x52\x41\x31\x0\x0\x2\x50\x17\xe7\x25\xe4\x86\x5\x72\xdf\x46\x38\x7\xf3\x79\xb1\xe4\xcc\x1a\x95\x61\xa4\x15\xe1\xb0\x47\x3\xdd\x7d\x28\x83\x2\xbd\x80\x7b\x82\x53\x32\xe8\xb\x61\x58\x3c\x2\xcd\x9d\x58\x7a\x3b\xc9\x57\xc6\xd8\xf1\xb5\xe2\xd7\x59\x2\xf7\x46\xc6\xa5\xd1\xf5\x8d\x2c\xdc\x70\xf7\xd0\x7d\x9e\x38\xf8\x23\xa8\x82\xae\xcf\x15\xd0\x14\x98\x0\xf4\xdd\x89\xda\x32\x7e\x7\x59\x44\x4f\x29\xf3\x5b\x27\xe6\xe4\xb6\xd6\x47\xf3\x2d\xbf\x16\x80\x84\x3\x3f\x29\xbb\x7e\xa8\x32\xd4\xb\x23\xcc\x0\x93\x53\x98\x7\xed\xc7\xf8\xc4\xc4\x6f\x7d\x89\xf6\x6f\x78\xe4\x31\x71\xca\x25\x68\xf6\x11\x2e\x72\x7\xbb\x70\xe\x6e\x3e\x17\x2f\x74\xc6\xea\xfb\x9d\x42\xf\xfc\x54\x43\xb5\xef\xe3\x7d\x51\xb6\x10\xc6\x8b\xe\x2\x5\x71\xe9\x6b\x12\x19\xf7\xdd\x93\x77\xf6\x41\xc2\x69\x39\x8c\xa7\x36\x50\xf8\x85\xa5\xdb\xad\x18\x95\xad\x5e\xaf\x54\xd7\x68\x15\x1d\x42\xf1\x5b\x80\x4f\xf9\xae\xf4\x39\x14\x55\x4f\xe0\x7f\x52\xb2\xbf\x74\x41\x75\xf3\x5d\xc7\x77\xeb\x38\x78\x53\xc3\xd0\x55\xcb\xb8\xf3\xe2\x96\x29\x2e\x9e\xb\xde\x9e\x88\x2f\xfc\xa4\xa2\x98\xf8\xd\x21\x6f\xb5\xb2\xf1\x1e\xc7\x3a\x10\x2\xcf\x46\x6\xdb\x8\xe6\x9c\x93\xfd\x7f\xd9\x35\xfd\x1e\x8b\xff\xfb\xef\x32\x23\x59\x37\xc4\x8\xe4\x4c\x6\x3d\x76\x11\x1e\x62\xae\xbc\x5c\xc9\x65\x82\x35\x81\x82\x19\x14\x3f\x32\x9\xc0\x4f\x17\x48\x96\x8d\x5c\xdc\x30\xa5\x5b\x7c\x9\xda\x3\xba\x83\xf0\x24\x49\xf\x4a\x1\x5f\xfc\x88\x42\x46\x2c\x4b\x3f\x1e\x2d\xad\x37\x44\x2f\xf\x8b\xd6\x1\x90\xfa\x8a\x1a\x90\xe1\x5f\xc2\x14\xb8\x5e\xa1\xec\x56\x99\x12\xcb\x10\x59\x7e\xae\x58\xc6\xd2\xd6\x91\x33\x8b\x17\x83\x6e\xc\x77\xea\xcf\x8a\xa\x5e\x3f\x7a\xe4\xed\x4a\x1d\xc7\xc9\xa5\xf4\xbd\xaa\x2c\xe1\x47\x9b\xed\x6\x45\xc9\x46\x45\xc3\xc8\x38\x30\x7e\x90\x2a\xe6\xfd\x3e\xac\xc1\x94\x76\x46\xc2\xb5\x2e\xb3\x2a\x6e\x90\x91\x79\x76\x80\x37\xee\x10\x37\x2\xd1\x5f\xcc\xc9\x72\x2f\xd3\x77\xfd\x6a\x71\x9c\xfd\xaf\x87\x8\x1e\xce\x17\x2b\xe7\x61\x39\xd4\x9c\x2f\xd4\x57\x4e\x48\x46\x22\xc0\xba\x24\xaa\x53\x6c\x90\xa2\x47\xd3\xa7\xee\x29\x1c\x7e\x8b\xc0\x96\x69\x29\x93\xbc\x5d\xfb\xed\x68\xf8\xdb\xb9\xf2\x9\x88\x50\x51\x2c\xef\x7f\xd5\x41\x66\xe9\xc\xe3\xd7\x69\x95\x83\xb3\xcb\x80\xa8\xec\x86\x92\x2e\x7b\xb2\x5e\xd1\xef\xac\x1e\xce\x6b\x2\x25\x1\x6e\x4d\x93\x41\x5e\xb3\xbf\x2f\x40\x41\xbd\x54\xfc\xdc\xf\xbc\xbc\x83\xa3\x96\x12\x94\x64\x49\xf5\x65\xb8\x93\x93\x23\xb\xc6\x5d\x37\x8b\x0\x1\x0\x1",
        @rsa1024_pub = "\x55\x52\x41\x31\x0\x0\x0\x90\x88\x1d\x28\x4a\xd3\x77\xfd\x6a\x71\x9c\xfd\xaf\x87\x8\x1e\xce\x17\x2b\xe7\x61\x39\xd4\x9c\x2f\xd4\x57\x4e\x48\x46\x22\xc0\xba\x24\xaa\x53\x6c\x90\xa2\x47\xd3\xa7\xee\x29\x1c\x7e\x8b\xc0\x96\x69\x29\x93\xbc\x5d\xfb\xed\x68\xf8\xdb\xb9\xf2\x9\x88\x50\x51\x2c\xef\x7f\xd5\x41\x66\xe9\xc\xe3\xd7\x69\x95\x83\xb3\xcb\x80\xa8\xec\x86\x92\x2e\x7b\xb2\x5e\xd1\xef\xac\x1e\xce\x6b\x2\x25\x1\x6e\x4d\x93\x41\x5e\xb3\xbf\x2f\x40\x41\xbd\x54\xfc\xdc\xf\xbc\xbc\x83\xa3\x96\x12\x94\x64\x49\xf5\x65\xb8\x93\x93\x23\xb\xc6\x5d\x37\x8b\x0\x1\x0\x1"
      ],
      [
        @rsa2048_priv = "\x52\x52\x41\x32\x0\x0\x4\x90\x89\x5f\x1\x4\xcb\x7d\x88\x9\xa1\xf0\xab\x98\xbc\x7b\x8c\xf2\x12\xda\xa0\xf2\xf5\x4f\x9d\x45\xb7\xc\x2b\xe5\x84\xc1\xd6\x51\xf3\x6e\x30\x37\xee\x32\xae\xf3\xf3\xaf\xeb\x43\x9d\xe8\x4b\xb6\x5d\x80\xfd\x98\xd8\x52\x89\xeb\x51\x2b\x81\x56\xca\x20\xd2\xa0\xf3\xca\x88\xe2\x55\xda\x41\x4b\xdf\x63\xe8\xab\x4f\x22\x5\x3a\x52\x14\xc5\x39\x4\x52\xc9\xee\x6c\xd9\xc1\x9a\x1b\x73\xe9\x1\x3e\xde\xb9\xce\x55\xa3\x66\x55\x93\x40\xaa\x2f\x7b\xcb\xe6\x1\xff\x7b\x93\xb1\xd6\x67\x4\x45\x5c\xfb\x9a\xa5\x2b\x9b\xed\x1b\xed\x3f\xde\xff\xb1\x3e\xb8\x93\xdf\xab\x7c\x1c\x97\xc9\x6f\x11\xab\xed\x49\x16\x38\x18\x40\x9a\x7\xff\xf3\xf\x29\x71\x3a\x79\x5\x44\x7c\xc2\x98\xe0\x5b\x14\x35\x66\x2d\x64\x84\x90\xb1\x78\x4d\x55\xe3\xb3\x37\xc5\x24\xec\x5a\x84\x38\x7d\xbb\xcd\xa5\xb9\x68\x78\x3c\xc6\x7e\x17\xab\x70\x42\x2\x14\x4a\xc0\x42\x8a\xc9\x72\x6d\x6e\x36\x53\x80\x67\x3\xc9\xd1\xb\xa1\x7d\x19\x7c\xef\x77\x61\xc3\xfb\xdc\x3a\x41\x26\x72\x4\x53\x5d\x92\x23\x76\xf0\x30\xa2\x66\xb6\x9e\x1a\xd5\xe4\x26\x5b\x3e\x15\x1a\x74\xcf\xda\xa7\x75\x77\x61\xf5\x7\xaf\x96\x8e\xc0\x66\x7c\xde\x4d\x11\x42\x51\x5c\xfc\x8b\x7f\xfb\xf8\x57\xf0\x23\xf5\x18\x6c\x77\xe3\xc7\xf1\x8d\x15\xc1\xcf\x3a\x78\x0\x5a\xd0\x27\x68\x6d\x81\x81\x51\x33\xde\xc4\xe5\x50\xf6\xfc\xc1\xc6\xec\x58\x86\x35\xd\xb8\xe\x26\xd1\x36\x45\x3f\xb6\xf9\x9d\x53\x1e\xe6\x59\x78\x2f\x5c\xec\x9\xe4\xdd\x14\xdc\xac\xbf\xa6\x6d\x2d\xc2\xd\x5e\x49\x51\x9\x91\xac\x7a\x78\x64\xb6\x86\xaa\xf3\x74\x74\x96\x34\xa1\xce\x9b\x82\xc4\x47\xd\x62\x95\xb6\xa7\x1\x68\x20\x4e\xce\xef\x62\x75\x33\x48\x8e\xed\xec\xe\x56\x97\xd9\x2d\xb2\xa7\x18\x56\x33\x60\x1c\xa0\x5c\xe1\x33\x72\x14\xde\x69\x12\xae\xc3\x7d\xf8\xae\x4\x1a\x90\x75\xb7\x8d\xb1\xe9\x82\x1b\x82\x8f\xd7\x72\x41\xd3\x31\x70\xb\x88\x89\xf5\xef\xb4\x98\x75\x46\x59\x2d\x4c\xe5\x4e\x1c\xe1\xf4\x24\x1e\xfd\x41\xc\xbf\x59\xc7\x99\x70\x30\x26\xcc\x6b\x82\xf4\x40\x59\x41\x8e\x8d\x53\xce\xd0\xa9\x95\x5a\x6f\x2b\xc4\x45\xa7\x39\x70\xdb\x1c\x50\xd6\x70\xce\x62\xf3\x9c\x52\xdf\x5\xfd\x83\xca\x4\x32\x48\xc\xb8\x92\xfc\xb0\x29\x46\x5f\xf4\x4\xff\xa\x53\x89\x49\x16\xd\xcd\xbe\xae\xac\xe3\x39\xfb\x8a\xd6\x59\x26\x7f\x3d\xac\xe3\x1f\x64\x3b\x80\xbe\x2e\x96\xcb\xfb\xe9\x3b\xa7\x75\x81\xe5\xa9\xee\xf9\x4c\x1d\x22\x64\x7\xa5\xc5\x48\xcf\x4b\xf7\x36\xdf\x3b\x8c\x90\xd0\x20\xd7\x16\xd0\xe4\x52\xae\x5\xee\x9\x5e\x6\x6f\x31\x6d\x37\xc9\xd3\xb3\x84\x21\xbc\x79\xbb\x49\xc4\x97\xe0\x6a\xb6\x8f\x91\xa9\x40\x64\x74\x67\x32\x0\xc4\xae\xc9\xa8\xf7\x81\x6a\x11\xbb\xa4\x54\xd2\x28\xc8\xca\xb3\xa0\x1\xa3\x9f\xde\x74\x1\x93\x36\xdc\x34\x26\xcd\x9a\xa1\xa7\x8b\xb0\x6c\xd1\x10\xe8\x3d\xff\x86\x48\xa5\xa1\x1e\x37\x1e\x5a\xe5\xf1\xfe\x75\xa\xd0\x42\xfa\x63\x20\x39\x70\x3b\x17\x92\xaf\x2c\x8f\xe8\x3e\x8b\x61\x90\xce\x6\xd6\x65\x23\x13\x45\xef\xb0\x7c\xcb\xf6\xad\x9a\xf5\xde\x43\x85\x32\xd2\xb4\x51\xeb\xa2\x9a\xa2\xfc\x6f\x91\xe0\xa5\xc6\x19\xf6\x7e\x3b\x59\x33\xb9\xb3\xae\x87\xa4\xa2\xef\x41\xa8\xca\x2a\x94\x87\x91\x7d\xe7\xa\x4b\x62\x35\x72\xf2\xc4\x48\x8d\x50\xa4\xeb\x53\x7c\xf6\xe1\xdd\x75\xa7\xe2\x2e\x8c\x4b\x9c\xa6\x58\x72\xe7\xe4\x3d\x61\xd7\x6e\x8b\xf5\xc7\xa6\x9f\xb1\xad\x48\x39\xae\x60\x57\x90\xa\x98\xbc\x6a\xaa\xd5\x2\xcc\x4\x50\xdc\xd0\x5e\xef\x44\x90\xb7\x7\xbd\x46\xdc\xf9\x9e\x87\x6f\x71\x50\x70\x8d\x3b\x2a\x12\x40\x75\xb5\xf0\x70\x51\x7f\x13\xee\x8f\x9a\x56\xe7\x3c\x7e\xf7\x38\x0\x48\x7a\x12\x2f\x9d\x8e\x51\xe7\xe9\x2\x3f\xe3\xbb\xa5\xf5\x89\x92\x68\x8a\x9e\xcd\xc0\x66\x8a\x45\x35\xe1\x3e\x7\xc9\x97\x98\x60\xf\x26\xc8\xf\x2b\xd8\x61\x4f\xc8\x84\x83\x21\x34\x74\x2\xca\xdb\xed\x2\x38\x43\x77\x77\xb\x2b\xda\x42\x2a\x28\x3a\x17\x1e\x8c\xed\x3e\x1f\x6d\xa7\xe1\xf0\xcf\x2c\x52\x39\x93\xf3\x51\x8b\x3d\xd7\x7b\x44\xe0\x9a\x3\x5e\x3c\x55\xe0\x7a\x25\x93\x5f\xeb\xfe\x3a\xc2\x31\x23\x69\xce\x74\x82\x67\x42\xf3\xb3\x1b\xec\x5c\x6c\x44\x45\x9d\xf7\xcb\x1c\xcc\xe1\xd\xc8\xa9\x5e\x23\x96\x9b\xe\x64\xdd\xd1\x15\x6c\xc8\x7b\x96\x6c\xfe\xd5\x15\x8e\x7a\x8c\x26\xee\x83\x93\x8d\xe\x59\x82\x58\x67\xab\x7a\x7e\xf7\xa\x29\x79\x48\xdb\xf1\xf7\xca\x40\x6e\x75\x52\xf2\x34\x5f\x78\x4a\x67\xc7\x42\xd1\xf7\xd1\xb\x24\x7d\xfa\xc9\xd4\x85\x1d\x32\x59\xdc\x9a\x74\x2a\xeb\x65\x34\x3d\x28\x20\x6a\x37\x5e\x5d\x40\x58\x63\x72\x46\x54\x91\xc7\x8d\x1f\x31\x51\x37\x33\x80\x7a\xf0\x45\xad\x77\x8d\xd7\xc5\xc9\x82\x35\x8c\xf1\x1b\x34\x9c\xd9\x88\x87\x9d\xa3\xf6\x63\xa1\x83\x27\xaf\x8c\xa2\x8d\x4d\x77\x78\xfe\x2a\x7\x42\x58\xa2\xc6\xed\xb4\xb3\x47\x38\x16\x4a\x30\x3c\x7c\xcd\x29\x3\x13\x47\xb5\x8a\x46\x2a\xdb\x1c\x0\xd5\xb2\x7b\xba\x8f\xca\x79\x4b\x8c\xe1\x80\xc4\x11\x60\xab\x9\xe\xeb\x8\xa4\x9f\x39\xe9\x94\xeb\xe9\xb\xb0\xd1\xaa\x94\x9c\x35\xae\x51\x5d\xbf\x0\x17\x40\xa0\x6d\x53\xd5\x0\x1\x0\x1",
        @rsa2048_pub = "\x55\x52\x41\x32\x0\x0\x1\x10\x27\x42\xc9\x6f\xe1\xf0\xcf\x2c\x52\x39\x93\xf3\x51\x8b\x3d\xd7\x7b\x44\xe0\x9a\x3\x5e\x3c\x55\xe0\x7a\x25\x93\x5f\xeb\xfe\x3a\xc2\x31\x23\x69\xce\x74\x82\x67\x42\xf3\xb3\x1b\xec\x5c\x6c\x44\x45\x9d\xf7\xcb\x1c\xcc\xe1\xd\xc8\xa9\x5e\x23\x96\x9b\xe\x64\xdd\xd1\x15\x6c\xc8\x7b\x96\x6c\xfe\xd5\x15\x8e\x7a\x8c\x26\xee\x83\x93\x8d\xe\x59\x82\x58\x67\xab\x7a\x7e\xf7\xa\x29\x79\x48\xdb\xf1\xf7\xca\x40\x6e\x75\x52\xf2\x34\x5f\x78\x4a\x67\xc7\x42\xd1\xf7\xd1\xb\x24\x7d\xfa\xc9\xd4\x85\x1d\x32\x59\xdc\x9a\x74\x2a\xeb\x65\x34\x3d\x28\x20\x6a\x37\x5e\x5d\x40\x58\x63\x72\x46\x54\x91\xc7\x8d\x1f\x31\x51\x37\x33\x80\x7a\xf0\x45\xad\x77\x8d\xd7\xc5\xc9\x82\x35\x8c\xf1\x1b\x34\x9c\xd9\x88\x87\x9d\xa3\xf6\x63\xa1\x83\x27\xaf\x8c\xa2\x8d\x4d\x77\x78\xfe\x2a\x7\x42\x58\xa2\xc6\xed\xb4\xb3\x47\x38\x16\x4a\x30\x3c\x7c\xcd\x29\x3\x13\x47\xb5\x8a\x46\x2a\xdb\x1c\x0\xd5\xb2\x7b\xba\x8f\xca\x79\x4b\x8c\xe1\x80\xc4\x11\x60\xab\x9\xe\xeb\x8\xa4\x9f\x39\xe9\x94\xeb\xe9\xb\xb0\xd1\xaa\x94\x9c\x35\xae\x51\x5d\xbf\x0\x17\x40\xa0\x6d\x53\xd5\x0\x1\x0\x1"
      ],
      [
        @rsa4096_priv = "\x52\x52\x41\x34\x0\x0\x9\x10\x55\x86\x6a\xdd\x12\xae\x82\x1b\x2c\xc7\x8e\x7\xc5\x8\xe0\x27\xf5\x99\x94\x78\xf8\x6c\xf4\x9f\x16\xce\xb3\x2f\xdd\xff\x44\xac\x50\x7f\xc5\x6f\x2a\x8f\xbe\x85\x21\xc3\x7f\x3c\x8b\x7c\x32\xe0\x36\xa9\xa\x46\x39\xec\x23\xfc\x3a\x28\xfb\x43\x49\xb2\x1\xec\xb\xbc\x1d\xbb\xc4\xe7\x15\xe6\x27\xa2\xe2\x1b\xa5\x22\xa4\xe0\xf9\xf5\x49\x8d\xbe\x30\x7f\xcd\xf1\x72\xa1\xca\xd7\x13\xf\xc\xe2\x45\x5d\xb4\x7d\xef\x37\x3f\x86\xdc\x66\xf5\x62\x83\x1c\xfe\xe7\xee\x56\x26\x59\x8f\xa1\xd3\xec\xec\xc3\x6a\x42\x5\x97\x18\x85\xa5\x53\x2\x33\x17\x6e\xe\x84\x4d\x9d\xe1\x89\xcf\xb\x68\xd6\x6\x35\xd6\x18\x22\x83\xe0\xf5\x66\x65\x7a\x42\xa8\x54\x47\x44\xe0\x6b\xa5\xe7\x3a\x4d\x86\xae\x30\x5\x2f\x2f\x79\xe1\xc2\xa9\xec\xcf\x90\xca\xb8\xa9\x6e\x2c\x1d\x83\x80\x89\x8c\xa0\xea\xa6\x7\x9d\x3c\x7d\x3e\xa6\xc7\xe6\xb0\x50\xee\xad\x4d\x35\x6b\x4f\xda\x64\x4\x8f\xeb\x7d\x81\xc6\x69\xc4\x9\xd\x6e\xaf\x7e\x1a\xda\x6f\xa6\xb7\x12\x1d\xf6\x15\x7e\x62\x7f\x25\x25\x7d\x91\x87\x43\x2\x9d\xd4\x35\x75\x83\xb2\x96\xb2\xae\x21\x5e\x96\x96\xdd\xf9\xe4\x99\x1c\x18\xe\x5c\xe5\x61\xdc\xe6\x30\xb2\xff\x3\x3c\x42\xa4\x64\x96\x64\x14\xd1\x82\xb7\x60\x29\x2b\x94\x53\x95\x40\x5d\x22\x68\x73\xde\x6\xb7\x23\x5d\xc4\xbe\xbe\x22\x5\xc4\x9d\x54\xcf\x3a\xa4\xef\xdc\xa7\xed\xf3\x98\x5c\x27\xb\xb\x6a\x1e\xf3\xb7\xfb\x1c\x50\x37\x5d\x33\xb7\x44\xfc\xb1\x58\x4f\x2c\x82\xbf\xd5\xf4\xab\x5e\x1\x92\x8a\x1d\x36\x43\x82\x59\xf1\xf\x46\x37\x94\x46\x4e\xdd\xe9\x24\xf\x56\x8b\x13\x3b\xb0\x96\x75\xb4\xa7\x55\x38\x71\xd4\x22\x4a\xc7\x9b\x76\xb3\x64\x5b\xe2\x41\x73\xb7\xf0\x2e\xc4\x50\xfc\xca\x96\xc1\xfe\x9\x4d\x35\xa4\xe1\x51\x69\xd8\xcb\x14\x90\x78\xd3\xe1\x3f\x38\xdb\x83\xbd\x6f\xb3\x50\x72\xe8\xc\xa7\xf3\x2c\xe8\xc2\xf4\x65\xe2\xa5\xb8\x8e\x5\x8a\x4c\x5d\xaa\xe6\x9\xfd\x4e\x1e\x9e\x88\xf0\x18\x62\x72\x4a\x49\x3c\x6d\x67\x2f\xf5\x20\xa7\xdd\xc3\xbc\xf8\xb8\xc9\x6a\x72\x89\xdd\xfb\x35\xc0\xc0\xd2\x37\x24\xef\x82\x56\x91\xba\x95\x89\x39\x1b\x55\x99\x10\x6b\x47\xf\x8d\xc1\xc8\xa\xd7\x31\xa8\xd\x63\xae\x6e\xb4\x4e\x9e\xf8\xa9\x1e\xf8\x6f\xec\xa3\xe1\x47\xb\x13\x49\xc7\x95\xf7\xd6\xbd\xe8\x79\x75\xf2\xa3\x9c\xec\xb2\x17\xf3\x20\x37\x55\x44\xa0\xb4\x69\x70\x44\xa4\x6b\xa\x8a\x6\x3f\x2c\x85\x7a\xfe\x17\x5a\x8\x52\x4d\xbc\x3f\xa9\xb5\x53\xf2\x0\xa2\xc2\x55\xa7\xa9\x5a\x37\x5f\x69\x71\x0\x65\x11\x78\x76\x21\x2d\x98\xf7\x3b\xa2\x8e\xaa\x95\xf8\x68\x69\x95\x38\xf1\x8d\x64\x80\xf2\x64\x6c\xfd\x87\x3b\x1d\x7a\xb1\x73\x3b\xb1\x86\xf\xa5\x42\x5\x8f\x84\x39\x40\xdd\x7f\xf1\xa1\xd6\xb3\x82\x97\x4b\x6e\x2d\x3c\xda\xb8\x3d\x59\x9d\xcb\xed\x51\x92\xd2\xd0\x6a\x9b\xce\x24\x72\xbb\x7a\x5c\x36\x35\xbd\x6f\x8\xcd\xe6\xbe\x91\x61\xec\x93\xcf\x1d\x84\x28\x9a\x63\x7e\xfa\xe9\x3e\x42\x3e\x7c\x69\x4c\x44\x6a\xf5\x84\x1b\x8\xca\xce\xae\x26\xbe\x7a\xc3\xa2\xfa\xe3\x96\xa9\xb0\xd0\x59\xb4\xce\x45\x6b\x82\xf\x83\x40\x2c\x67\x1a\x26\x69\xe8\x81\xe9\x85\x5c\x2d\x15\xcb\x98\x7b\x50\xdf\x12\xe\x70\x9a\xf0\xe6\x45\xf9\xca\x35\x8\xf3\xa0\xc4\x77\x99\xcb\x69\x2c\x7\xec\xf8\x74\xdc\x12\xe3\x70\xd8\x3f\xea\xbf\xc7\xac\x1c\x6e\xe\x91\x6f\x73\xc0\x26\x58\xe6\xd7\x9a\x96\x53\xa\xa5\xc7\x16\x62\x7b\xcf\xd0\x96\xa\x61\x9c\x72\xe2\x67\xec\x4a\x8\x63\xdd\x5a\xb6\x8a\xb9\xa5\xf2\x43\xda\x51\x70\x34\x51\x1a\xc6\x48\x56\x65\x2c\x4d\x5f\x14\xb3\xf2\xb5\x82\x6f\xcc\x4\xae\xcd\xc9\x4f\xf6\x7c\xf2\x51\x5c\x8a\x7c\x57\x2d\x52\x29\xc4\x6\xd4\x90\xce\xad\x2b\xf4\x1f\xe6\x43\x83\x8d\x93\x58\x90\xf9\x29\xa8\x31\x61\xb7\x2\x8d\x8d\xfe\x89\x9b\x12\xfb\x9\x41\x9b\x12\x85\xe0\x93\xe1\xcf\x87\x1c\xe5\xd0\xc8\xeb\x1b\xb5\x8\xba\xe9\x78\x11\x56\x3c\x89\xac\x77\xda\x67\xf2\x12\x52\x7f\xea\xbc\x87\x61\x42\xbd\x5b\x97\xe0\x3a\x62\x7f\xcd\x55\x7b\x22\x97\xad\xcb\x8d\xe2\x43\xef\xf7\x50\xf6\x45\x11\x75\x86\xcf\x1a\x1e\xba\x4e\x44\x9c\xde\x7c\x19\xb\x8d\xbb\x41\xa4\x2d\x43\x82\xc3\x3f\x69\x96\xc1\xfe\x6b\xc6\x8d\x15\xb7\x39\xb1\x70\x52\xad\xa5\x31\xfb\xfd\x30\xbe\x91\x22\x96\xde\xab\xd3\xb4\x27\x4b\x40\x34\x85\xf4\x64\xe0\x9c\xa0\x41\x57\x41\x58\xbe\xc3\xd1\x84\x92\x48\xc3\x32\x22\xa3\xe6\xb0\x3d\xee\x6d\xbc\x8\xb5\x45\xc9\xd1\xc7\x14\x75\x4d\x4e\xe1\xd3\xa7\xa8\xe9\x62\xf4\x5b\x7\xb9\x7f\x79\x70\x6b\xfb\xc3\xfc\x4\x8d\x3b\x8e\x3\x45\xc4\x9d\x3c\xa1\xb1\x70\xed\xc\x4b\xbe\xeb\xb9\xe9\x29\x2e\xf0\xd1\xee\x1b\xd7\x1e\xe2\xe6\x1e\x18\x3a\xf9\xad\x75\x45\x8a\x38\x74\x78\xc8\xa2\xae\x2b\x12\x13\xfc\x9b\x46\xc1\xf0\x15\x91\x75\x63\x3d\xb7\x1b\xa3\xce\x33\xf9\x74\x9f\x7d\x24\x45\xce\xaa\xf3\x42\x86\x19\xc6\x4e\x54\x57\xcd\x4e\x46\xb9\x1e\x24\x79\xcd\xf0\x68\xbe\xf2\x49\x76\x6e\xd\xc4\x8f\x4c\x5a\x85\x57\x1d\xbb\x76\x70\xa6\xd3\xb6\x84\x1c\x13\x77\x8c\x85\x6d\x76\x62\xd5\xfc\xb0\xf7\xb6\x5f\xe4\xc2\xd9\xdc\xc\xb5\x3d\x9b\x64\xa4\x3\x8c\xa8\x94\x88\xb8\x41\xcb\xb3\x6c\xf8\xf0\xa7\xed\xa9\x1b\x1e\x6d\x1\x10\x34\xd4\x4b\xf1\x87\x7e\xa0\x93\xac\x2c\xa1\xcc\x75\xf5\x3e\xc8\xd9\x6f\xa7\xfc\xe\x78\x28\x4d\x1a\xfa\xd7\x2f\x3c\x7\xe7\x9e\x4d\xb1\x34\x14\x60\x44\x51\x3b\xe1\xc1\x14\x12\xc4\xed\xbe\xd8\xb2\xc1\x7b\xf9\x45\x68\xdd\x1e\xdb\x15\x5d\x6e\x85\xb0\x51\x56\xbc\x6f\x65\xb2\xce\x59\x98\xfe\x5\xd5\xf0\xca\x99\xa3\xb5\x4b\x10\x51\x49\xa6\xc9\x7\xb0\x2e\x86\x3d\xe0\xc6\x3d\x4c\x60\x6c\xd6\xb1\xba\x89\xe2\xc1\x9a\x26\x8f\x44\x15\x6f\x2d\xaf\x4f\x22\xc5\xf3\x5b\xa7\x4b\x57\x83\xb2\xeb\xbe\x7c\x90\xb1\xcc\xa8\xdd\x37\xa9\xf2\x9b\x62\xd0\x2c\x71\x97\x8a\x9d\x43\x6c\xf9\xc9\xd7\xf4\xed\x56\xb3\x26\xab\x6a\xa3\x17\x2\x73\xa7\x57\x2e\x9f\x9e\x50\x31\xba\xf7\x2e\xda\x18\x20\x87\x43\xe2\x86\x9b\x9e\x1\x57\x84\xe0\xaa\xdb\x45\xd7\x5b\xe1\x44\xf7\xa2\x75\x82\x8e\x27\x71\x71\x9c\xce\x10\x32\x78\xc2\x1d\xc8\x6\x3\x40\x22\x92\x2b\x42\x76\x4c\x21\x40\x82\x3f\xb4\x6e\x87\xa6\xc6\xb8\xa1\x9f\x92\x78\xb0\x8e\xa5\x39\xb2\x85\x2c\x46\x43\xd7\x15\xd2\xa4\xb7\x2\x2a\xe4\xf8\xd\x41\x8f\x6d\x2f\xec\x48\xd8\xfc\x20\x1d\x70\x4f\x3c\xa0\x84\x27\xc2\x67\x27\xd1\x5b\x52\x57\x77\xdc\xd1\xf4\x1b\x92\x6b\xdf\x87\x7a\x60\x1f\x2e\x47\xdb\x3\x9b\xff\x36\xac\x64\xd4\x7a\x37\x81\x94\xd6\xf7\xcc\x22\xaa\xfa\x7a\xca\xf4\x86\x49\xde\x1\x65\x37\xf0\xc9\xe\x18\x32\x21\xdb\xef\xb3\xbe\xba\x43\x3d\xfc\x5b\xbe\xf6\x74\xda\x3c\x40\xd\xbc\xb8\x33\xca\x91\xe\x8f\x35\x73\xd8\xa0\x73\xd8\xa5\x50\xb\xa2\x21\xec\x9b\x1\x73\x67\xa4\xaf\x7a\x3e\xc\xe3\x79\xc7\x19\x21\x9e\x15\x88\xcc\xe5\x24\xc1\xd\x4b\x43\x30\x57\xc5\xaf\x7a\x7d\x9f\x81\x4a\x6e\xe0\x84\xff\x43\xd3\x33\x24\xc5\x58\xce\x5\xdb\xd8\x57\xa5\x1d\x3\xdf\xfa\xfe\x55\x5d\xa7\x3b\x7a\xde\x7d\x64\xd4\x6c\xf5\x4e\xc3\xb6\xaa\x57\x84\xcb\xae\x6e\x8b\xe9\x29\x46\xc4\x96\x5\xf8\xcf\xcd\x2a\x65\xe3\xdd\x65\xb3\xb4\xbf\xf6\x3d\xe4\x8a\xce\xdd\xb2\xd2\x54\x41\xc4\x13\x98\xe9\x1f\x8\xc6\x91\x32\x17\xcd\x2a\xc6\xa3\x55\x8f\x50\xba\xf3\x60\xf5\x61\xd7\x29\x59\x8\x61\xbf\xd3\x8b\x83\x46\x65\x28\xbd\x8e\x97\xc4\xa5\x50\x63\x3a\xcf\xfa\x2c\x24\x89\x42\x36\xf3\x3f\xc\x23\x72\x7a\xe8\xf7\x4e\xbe\xa\x6e\xe\x1c\x62\xc1\x52\x15\xe6\xce\x7f\x2a\x6a\xa9\xfe\x3f\x84\x27\xef\xf6\xfc\x4f\x12\x1f\x64\xbe\x85\xe5\x53\xa0\x67\xe\x98\x38\x4c\x2e\x1e\xbf\x51\xa\xc\x9d\x70\x81\xe5\x3a\x41\x31\x5e\x9f\x26\x4a\x66\x91\x72\x17\xbf\xab\xea\xea\x8b\xbc\x47\xb2\xd9\x7d\xd7\xe\x41\x99\xc2\xd\xe5\x5f\x54\xea\x9f\x22\xe1\x9d\xf1\x22\x7d\x5d\xa6\x9d\x34\xc5\xf7\x59\x9d\xec\x5f\xec\x70\x9c\xb9\xe5\xea\x5c\x27\x32\x34\xa9\x90\x76\x21\x8e\xa6\x0\x6a\xc9\xef\xbc\x2d\xc\x26\x1e\xa0\x27\xe2\xd4\xec\xdc\xce\xfb\x56\x21\x9b\x4c\xa3\x20\x75\xf8\xc2\xf3\xee\xa\x89\xc8\x2f\x33\xee\x9a\x93\x67\xb1\xfb\xb8\x43\x38\xa6\xe5\xe1\x6\x86\x68\x54\xc0\xe2\x6a\xbc\x95\xba\xe3\xa4\xcf\x1a\x87\x5a\x10\x89\x7c\x68\xe0\xbd\x47\xf0\xae\xde\x8f\x8c\x6a\x33\xb1\x8c\xb6\xdc\xc1\x31\x7c\xac\xb4\x1f\xda\xe5\x77\x3f\xb1\x55\x13\x62\xc2\x7\x9c\x5a\x19\x3b\xfc\xac\xc\x96\x4f\xe0\xff\xc4\x3b\x99\x3a\xaf\xf4\x6f\x29\x27\xd0\xda\x5c\x3f\x5d\x33\x83\x6a\xdc\x5e\x54\xe6\x72\xca\x66\x4\x9\xac\xe6\xfa\xd\x59\xb0\x5a\x8d\x29\x6a\x81\x6a\x9b\xa5\xe4\xeb\x19\x9b\x7a\x60\xd0\x1a\xb0\xdb\x2d\xb5\xfd\x33\x3d\x31\xfc\xb7\x1c\xb6\xe\xc\x37\xde\xc1\x83\x27\xd6\xf2\x16\x1a\x7f\xdd\x44\xf5\x3\xd2\xb9\x2c\xe2\xb2\xfd\xdd\xc4\xc2\x27\x67\xd4\xde\x3a\x2f\x50\x3e\x7f\xeb\xb0\xb6\xe8\x16\xfd\xd0\xdd\xc0\x5e\x12\x28\x82\x9b\xa1\xbc\xf7\xe2\xac\xae\xac\xba\xc\xf1\x74\x4a\x17\x16\xcd\x4c\x5b\xe5\xd2\x35\xc7\x6\x7d\x87\xde\x13\x3f\xaa\x7d\x8a\xc6\xc0\x75\x50\x26\xb6\x27\x4c\x4b\xb9\xc3\xa7\x1a\x19\xe1\x21\xf0\x91\x84\x45\x65\xc9\xf2\x3b\x26\x54\xf0\x2f\x4a\xe7\x13\xe1\x5d\xbd\x71\x78\x18\xc8\x8e\x96\x7\x13\x44\xb2\x1b\x59\x68\x6f\x44\x47\x20\x96\xc\xd6\x54\x12\xdf\xd2\x2f\x95\x6b\xc3\xbe\xc7\x35\x8\xea\x30\x37\x9b\xdb\x14\x3b\x96\x17\x5f\xe4\xe\x34\x58\xcb\xb4\x92\x1f\x16\x36\xd8\x82\x2b\xf3\x72\xf4\x15\x36\xa4\x76\x7b\x26\x11\x45\xfb\xe8\x5c\x7d\xb4\x86\x19\xc0\x1f\xd4\xca\x3b\x74\x7a\xca\x5b\x44\x73\xf1\xa9\x8e\xc6\x8\x3\xcb\x7e\x4c\x17\x71\x3d\x36\x95\x6f\x49\xb1\x3a\xc\x27\x62\xd0\x8f\x2b\xfe\xaa\xa8\x9a\x95\x14\xb6\xe9\xe4\x5c\x2\xf9\xcb\xd8\xad\x6c\xac\xd9\xdb\x92\x98\xf\x7f\xa2\xa7\x82\x7e\xfd\x73\xba\x31\xcc\x96\x64\x3c\xeb\x1a\x76\x12\x14\xa8\x9d\x82\xbe\xf8\xda\x85\x64\xf0\x48\x68\xbe\xa8\x2\x27\x9\xd8\xfe\x93\xb2\xb3\x9c\x6\x90\x9\x2f\xe7\x6c\xa2\xb4\x3e\x6c\x1a\x4e\x1\xad\x7d\xfd\x4c\xd4\x6e\xe6\x3b\x1c\x5e\x9d\x28\x8\xe2\x2d\x23\x9f\xd3\x88\xdd\x70\x5d\x6f\xad\x99\x0\x53\xb1\xb4\x7f\xc5\x32\x76\xff\x25\x29\xcc\xa1\x0\x73\x6a\xff\x24\x54\x80\x7d\xd5\x6b\x4d\x1a\x1f\x8b\xb5\x0\x1\x0\x1",
        @rsa4096_pub = "\x55\x52\x41\x34\x0\x0\x2\x10\xf1\xa4\xce\xe7\xc9\xef\xbc\x2d\xc\x26\x1e\xa0\x27\xe2\xd4\xec\xdc\xce\xfb\x56\x21\x9b\x4c\xa3\x20\x75\xf8\xc2\xf3\xee\xa\x89\xc8\x2f\x33\xee\x9a\x93\x67\xb1\xfb\xb8\x43\x38\xa6\xe5\xe1\x6\x86\x68\x54\xc0\xe2\x6a\xbc\x95\xba\xe3\xa4\xcf\x1a\x87\x5a\x10\x89\x7c\x68\xe0\xbd\x47\xf0\xae\xde\x8f\x8c\x6a\x33\xb1\x8c\xb6\xdc\xc1\x31\x7c\xac\xb4\x1f\xda\xe5\x77\x3f\xb1\x55\x13\x62\xc2\x7\x9c\x5a\x19\x3b\xfc\xac\xc\x96\x4f\xe0\xff\xc4\x3b\x99\x3a\xaf\xf4\x6f\x29\x27\xd0\xda\x5c\x3f\x5d\x33\x83\x6a\xdc\x5e\x54\xe6\x72\xca\x66\x4\x9\xac\xe6\xfa\xd\x59\xb0\x5a\x8d\x29\x6a\x81\x6a\x9b\xa5\xe4\xeb\x19\x9b\x7a\x60\xd0\x1a\xb0\xdb\x2d\xb5\xfd\x33\x3d\x31\xfc\xb7\x1c\xb6\xe\xc\x37\xde\xc1\x83\x27\xd6\xf2\x16\x1a\x7f\xdd\x44\xf5\x3\xd2\xb9\x2c\xe2\xb2\xfd\xdd\xc4\xc2\x27\x67\xd4\xde\x3a\x2f\x50\x3e\x7f\xeb\xb0\xb6\xe8\x16\xfd\xd0\xdd\xc0\x5e\x12\x28\x82\x9b\xa1\xbc\xf7\xe2\xac\xae\xac\xba\xc\xf1\x74\x4a\x17\x16\xcd\x4c\x5b\xe5\xd2\x35\xc7\x6\x7d\x87\xde\x13\x3f\xaa\x7d\x8a\xc6\xc0\x75\x50\x26\xb6\x27\x4c\x4b\xb9\xc3\xa7\x1a\x19\xe1\x21\xf0\x91\x84\x45\x65\xc9\xf2\x3b\x26\x54\xf0\x2f\x4a\xe7\x13\xe1\x5d\xbd\x71\x78\x18\xc8\x8e\x96\x7\x13\x44\xb2\x1b\x59\x68\x6f\x44\x47\x20\x96\xc\xd6\x54\x12\xdf\xd2\x2f\x95\x6b\xc3\xbe\xc7\x35\x8\xea\x30\x37\x9b\xdb\x14\x3b\x96\x17\x5f\xe4\xe\x34\x58\xcb\xb4\x92\x1f\x16\x36\xd8\x82\x2b\xf3\x72\xf4\x15\x36\xa4\x76\x7b\x26\x11\x45\xfb\xe8\x5c\x7d\xb4\x86\x19\xc0\x1f\xd4\xca\x3b\x74\x7a\xca\x5b\x44\x73\xf1\xa9\x8e\xc6\x8\x3\xcb\x7e\x4c\x17\x71\x3d\x36\x95\x6f\x49\xb1\x3a\xc\x27\x62\xd0\x8f\x2b\xfe\xaa\xa8\x9a\x95\x14\xb6\xe9\xe4\x5c\x2\xf9\xcb\xd8\xad\x6c\xac\xd9\xdb\x92\x98\xf\x7f\xa2\xa7\x82\x7e\xfd\x73\xba\x31\xcc\x96\x64\x3c\xeb\x1a\x76\x12\x14\xa8\x9d\x82\xbe\xf8\xda\x85\x64\xf0\x48\x68\xbe\xa8\x2\x27\x9\xd8\xfe\x93\xb2\xb3\x9c\x6\x90\x9\x2f\xe7\x6c\xa2\xb4\x3e\x6c\x1a\x4e\x1\xad\x7d\xfd\x4c\xd4\x6e\xe6\x3b\x1c\x5e\x9d\x28\x8\xe2\x2d\x23\x9f\xd3\x88\xdd\x70\x5d\x6f\xad\x99\x0\x53\xb1\xb4\x7f\xc5\x32\x76\xff\x25\x29\xcc\xa1\x0\x73\x6a\xff\x24\x54\x80\x7d\xd5\x6b\x4d\x1a\x1f\x8b\xb5\x0\x1\x0\x1"
      ]
    ]
    @message = 'This is test message'
  end

  def test_encrypt_decrypt
    [
      ['', ''],
      ['', @ec256_pub],
      [@ec256_priv, ''],
      [@ec256_pub, @ec256_priv]
    ].each do |k1, k2|
      assert_raise(Themis::ThemisError) do
        smessage = Themis::Smessage.new(k1, k2)
      end
    end

    @keys.each do |k|
      smessage = Themis::Smessage.new(k[0], k[1])
      assert_raise(Themis::ThemisError) do
        encrypted_message = smessage.wrap('')
      end
      encrypted_message = smessage.wrap(@message)

      assert_raise(Themis::ThemisError) do
        decrypted_message = smessage.unwrap(encrypted_message + '1')
      end
      decrypted_message = smessage.unwrap(encrypted_message)

      assert_equal(@message, decrypted_message)
    end
  end

  def test_sign_verify
    assert_raise(Themis::ThemisError) do
      signed_message = Themis.s_sign('', @message)
    end
    assert_raise(Themis::ThemisError) do
      signed_message = Themis.s_sign(@ec256_pub, @message)
    end
    assert_raise(Themis::ThemisError) do
      signed_message = Themis.s_sign(@ec256_priv, '')
    end

    signed_message = Themis.s_sign(@ec256_priv, @message)
    assert_raise(Themis::ThemisError) do
      verified_message = Themis.s_verify('', signed_message)
    end
    assert_raise(Themis::ThemisError) do
      verified_message = Themis.s_verify(@ec256_priv, signed_message)
    end

    @keys.each do |k|
      signed_message = Themis.s_sign(k[0], @message)
      verifyed_message = Themis.s_verify(k[1], signed_message)
      assert_equal(@message, verifyed_message)
    end
  end
end
