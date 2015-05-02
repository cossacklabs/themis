#
# Copyright (C) 2015 CossackLabs
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

#echo client for rabbitMQ
from themis import ssession;
import pika
import uuid

client_priv = str('\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01');

server_pub  = str('\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0');

class transport(ssession.mem_transport):	#necessary callback
    def get_pub_key_by_id(self, user_id):
        if user_id != "server":			#we have only one peer with id "server"
            raise Exception("no such id");
        return server_pub;

session=ssession.ssession("client", client_priv, transport());

class SsessionRpcClient(object):
    def __init__(self):
        self.connection = pika.BlockingConnection(pika.ConnectionParameters(host='localhost'))
        self.channel = self.connection.channel()
        result = self.channel.queue_declare(exclusive=True)
        self.callback_queue = result.method.queue
        self.channel.basic_consume(self.on_response, no_ack=True, queue=self.callback_queue)

    def on_response(self, ch, method, props, body):
        if self.corr_id == props.correlation_id:
            self.status, self.response = session.unwrap(body)			#unwrap response message

    def call(self, message):
        self.response = None
        self.corr_id = str(uuid.uuid4())
        self.channel.basic_publish(exchange='',
                                   routing_key='ssession_queue',
                                   properties=pika.BasicProperties(
                                         reply_to = self.callback_queue,
                                         correlation_id = self.corr_id,
                                         ),
                                   body=message)
        while self.response is None:
            self.connection.process_data_events()
        return (self.status, self.response)

ssession_rpc = SsessionRpcClient()

res, response = ssession_rpc.call(session.connect_request());			#make and send session initialisation message and get unwrapped reply message
while res==1:									#if res==1 then session is not established yet
    res, response = ssession_rpc.call(response);				#send response message to client as is and get new unwrapped reply message

res, response = ssession_rpc.call(session.wrap("This is test message!"));	#encrypt,send information message and get unwrapped reply message
res, response = ssession_rpc.call(session.wrap("This is test message #2!"));	#encrypt,send information message and get unwrapped reply message
res, response = ssession_rpc.call(session.wrap("This is test message #3!"));	#encrypt,send information message and get unwrapped reply message
res, response = ssession_rpc.call(session.wrap("This is test message #4!"));	#encrypt,send information message and get unwrapped reply message


print response;									#print message
