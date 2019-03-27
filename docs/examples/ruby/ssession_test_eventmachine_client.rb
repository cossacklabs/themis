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

module HttpHeaders
  class CallbacksForThemis < Themis::Callbacks
    def get_pub_key_by_id(id)
      {
        'server' => "\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21",
        'client' => "\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"
      }[id]
    end
  end

  def post_init
    @count = 0
    @callbacks = CallbacksForThemis.new
    client_priv = "\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf"
    @session = Themis::Ssession.new('client', client_priv, @callbacks)
    send_data @session.connect_request
  end

  def receive_data(data)
    res, mes = @session.unwrap(data)

    unless @session.established?
      send_data mes
      return
    end

    puts mes
    if @count < 10
      send_data @session.wrap('test message')
      @count += 1
    else
      send_data @session.wrap('finish')
    end
  end

  def unbind
    if @data =~ /[\n][\r]*[\n]/m
      $`.each { |line| puts ">>> #{line}" }
    end
    EventMachine.stop_event_loop
  end
end

EventMachine.run do
  EventMachine.connect '127.0.0.1', 26260, HttpHeaders
end
