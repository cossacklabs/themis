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

$q1 = Queue.new
$q2 = Queue.new

class Memory_transport
    def initialize(to, from)
	@to=to
	@from=from
    end
    def send(message)
	@from.enq(message)
    end
    
    def receive()
	count=0
	while @to.empty?
	    if count > 5
		raise RuntimeError, "timeout"
	    end
	    sleep(1)
	    count=count+1
	end
	return @to.deq
    end
end

class  Callbacks_for_themis < Themis::Callbacks
    def initialize(pub_keys)
	@pub_keys=pub_keys
    end
    def get_pub_key_by_id(id)
	    return @pub_keys[id]
    end
end

class TestSession < Test::Unit::TestCase
    def setup
	@server_priv_key, @server_pub_key = Themis::SKeyPairGen.new.ec()
	@client_priv_key, @client_pub_key = Themis::SKeyPairGen.new.ec()
	@pub_keys = {"server" => @server_pub_key, "client" => @client_pub_key}
	@message="This is test message"
    end

    def client
	client_transport = Memory_transport.new($q1,$q2)
	session = Themis::Ssession.new("client", @client_priv_key, Callbacks_for_themis.new(@pub_keys))
	control_message=session.connect_request()
	while !(session.is_established)
	    client_transport.send(control_message)
	    control_message=session.unwrap(client_transport.receive())[1]
	end
	client_transport.send(session.wrap(@message))
	assert_equal(@message, session.unwrap(client_transport.receive())[1])
    end
    
    def server
	server_transport = Memory_transport.new($q2,$q1)
	session = Themis::Ssession.new("server", @server_priv_key, Callbacks_for_themis.new(@pub_keys))
	begin 
	    server_transport.send(session.unwrap(server_transport.receive())[1])
	end while !(session.is_established)
	assert_equal(@message, session.unwrap(server_transport.receive())[1])
	server_transport.send(session.wrap(@message))
    end

    def testSession
	threads = []
	threads << Thread.new do
	    server()
	end
	threads << Thread.new do
	    client()
	end


	threads.each {|thr| thr.join }
    end
end
