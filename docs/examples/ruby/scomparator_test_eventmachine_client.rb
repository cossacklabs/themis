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

module ComparationClient
  def post_init
    @comparator = Themis::Scomparator.new('Test shared secret')
    send_data @comparator.begin_compare
  end

  def receive_data(data)
    mes = @comparator.proceed_compare(data)
    case @comparator.result
    when Themis::Scomparator::NOT_READY
      send_data mes
      return
    when Themis::Scomparator::MATCH
      puts 'match'
    else
      puts 'does not match'
    end
    EventMachine.stop_event_loop
  end

  def unbind
    if @data =~ /[\n][\r]*[\n]/m
      $`.each { |line| puts ">>> #{line}" }
    end
    EventMachine.stop_event_loop
  end
end

EventMachine.run do
  EventMachine.connect '127.0.0.1', 26260, ComparationClient
end
