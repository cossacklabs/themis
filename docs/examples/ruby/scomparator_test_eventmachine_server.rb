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

module ComparationServer
  def post_init
    @comparator = Themis::Scomparator.new('Test shared secret' + '1')
  end

  def receive_data(data)
    mes = @comparator.proceed_compare(data)
    send_data mes
    if @comparator.result != Themis::Scomparator::NOT_READY
      if @comparator.result == Themis::Scomparator::MATCH
        puts 'match'
      else
        puts 'does not match'
      end
    end
  end
end

EventMachine.run do
  host = '0.0.0.0'
  port = 26260
  EventMachine.start_server host, port, ComparationServer
  puts "Started server on #{host}:#{port}..."
end
