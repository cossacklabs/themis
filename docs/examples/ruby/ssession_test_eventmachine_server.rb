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
require 'eventmachine'
require 'rbthemis'

module EchoServer
  class CallbacksForThemis < Themis::Callbacks
    def get_pub_key_by_id(id)
      {
        'server' => "\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21",
        'client' => "\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"
      }[id]
    end
  end

  def post_init
    server_priv = "\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01"
    @callbacks = CallbacksForThemis.new
    @session = Themis::Ssession.new('server', server_priv, @callbacks)
  end

  def receive_data(data)
    res, mes = @session.unwrap(data)

    if res == Themis::SEND_AS_IS
      send_data mes
      return
    end

    puts mes
    exit 0 if mes == 'finish'
    send_data @session.wrap(mes)
  end
end

EventMachine.run do
  host = '0.0.0.0'
  port = 26260
  EventMachine.start_server host, port, EchoServer
  puts "Started server on #{host}:#{port}..."
end
