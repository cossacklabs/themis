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
require 'rbthemis'
require 'test/unit'
require 'thread'

class MemoryTransport
  def initialize(to, from)
    @to = to
    @from = from
  end

  def send(message)
    @from.enq message
  end

  def receive
    count = 0
    while @to.empty?
      raise 'timeout' if count > 5
      sleep 1
      count += 1
    end
    @to.deq
  end
end

class TestComparator < Test::Unit::TestCase
  def setup
    @message1 = 'This is test message'
    @message2 = 'This is test message2'
    @server_result = Themis::Scomparator::NOT_READY
    @client_result = Themis::Scomparator::NOT_READY
    @q1 = Queue.new
    @q2 = Queue.new
  end

  def client(message)
    client_transport = MemoryTransport.new(@q1, @q2)
    comparator = Themis::Scomparator.new(message)
    control_message = comparator.begin_compare
    while comparator.result == Themis::Scomparator::NOT_READY
      client_transport.send(control_message)
      control_message = comparator.proceed_compare(client_transport.receive)
    end
    @client_result = comparator.result
  end

  def server(message)
    server_transport = MemoryTransport.new(@q2, @q1)
    comparator = Themis::Scomparator.new(message)
    begin
      server_transport.send(
        comparator.proceed_compare(server_transport.receive))
    end while comparator.result == Themis::Scomparator::NOT_READY
    @server_result = comparator.result
  end

  def test_comparator
    [
      Thread.new { server(@message1) },
      Thread.new { client(@message1) }
    ].each(&:join)
    assert_equal(@client_result, Themis::Scomparator::MATCH)
    assert_equal(@server_result, Themis::Scomparator::MATCH)

    [
      Thread.new { server(@message1) },
      Thread.new { client(@message2) }
    ].each(&:join)
    assert_equal(@client_result, Themis::Scomparator::NOT_MATCH)
    assert_equal(@server_result, Themis::Scomparator::NOT_MATCH)
  end
end
