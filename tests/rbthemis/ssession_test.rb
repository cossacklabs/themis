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

class CallbacksForThemis < Themis::Callbacks
  def initialize(pub_keys)
    @pub_keys = pub_keys
  end

  def get_pub_key_by_id(id)
    @pub_keys[id]
  end
end

class TestSession < Test::Unit::TestCase
  def setup
    @server_priv_key, @server_pub_key = Themis::SKeyPairGen.new.ec
    @client_priv_key, @client_pub_key = Themis::SKeyPairGen.new.ec
    @client_priv_key_rsa, @client_pub_key_rsa = Themis::SKeyPairGen.new.rsa
    @q1 = Queue.new
    @q2 = Queue.new
    @pub_keys = {'server' => @server_pub_key, 'client' => @client_pub_key}
    @message = 'This is test message'
  end

  def client
    client_transport = MemoryTransport.new(@q1, @q2)
    session = Themis::Ssession.new(
      'client', @client_priv_key, CallbacksForThemis.new(@pub_keys))
    control_message = session.connect_request

    until session.established?
      client_transport.send(control_message)
      control_message = session.unwrap(client_transport.receive)[1]
    end
    client_transport.send(session.wrap(@message))
    assert_equal(@message, session.unwrap(client_transport.receive)[1])
  end

  def server
    server_transport = MemoryTransport.new(@q2, @q1)
    session = Themis::Ssession.new(
      'server', @server_priv_key, CallbacksForThemis.new(@pub_keys))
    begin
      server_transport.send(session.unwrap(server_transport.receive)[1])
    end until session.established?
    assert_equal(@message, session.unwrap(server_transport.receive)[1])
    server_transport.send(session.wrap(@message))
  end

  def test_session
    [
      Thread.new { server },
      Thread.new { client }
    ].each(&:join)
  end

  def test_valid_key_private_ec
    assert_nothing_raised do
      session = Themis::Ssession.new(
        'client', @client_priv_key, CallbacksForThemis.new(@pub_keys))
    end
  end

  def test_invalid_key_public_ec
    assert_raise(Themis::ThemisError) do
      session = Themis::Ssession.new(
        'client', @client_pub_key, CallbacksForThemis.new(@pub_keys))
    end
  end

  def test_invalid_key_private_rsa
    assert_raise(Themis::ThemisError) do
      session = Themis::Ssession.new(
        'client', @client_priv_key_rsa, CallbacksForThemis.new(@pub_keys))
    end
  end

  def test_invalid_key_public_rsa
    assert_raise(Themis::ThemisError) do
      session = Themis::Ssession.new(
        'client', @client_pub_key_rsa, CallbacksForThemis.new(@pub_keys))
    end
  end
end
