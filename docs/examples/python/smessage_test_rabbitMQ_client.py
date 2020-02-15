#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License")
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

# echo client for rabbitMQ
import pika
import uuid
from pythemis.smessage import SMessage

client_private = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01"

server_public = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"

smessage = SMessage(client_private, server_public)


class SsessionRpcClient(object):
    def __init__(self):
        self.connection = pika.BlockingConnection(
            pika.ConnectionParameters(host='localhost'))
        self.channel = self.connection.channel()
        result = self.channel.queue_declare('smessage_queue_client', exclusive=True)
        self.callback_queue = result.method.queue
        self.channel.basic_consume(self.callback_queue, self.on_response, auto_ack=False)

    def on_response(self, ch, method, props, body):
        if self.corr_id == props.correlation_id:
            # decrypt acceptedd message
            self.response = smessage.unwrap(body)

    def call(self, message):
        self.response = None
        self.corr_id = str(uuid.uuid4())
        self.channel.basic_publish(
            exchange='', routing_key='smessage_queue_server',
            properties=pika.BasicProperties(
                reply_to=self.callback_queue,
                correlation_id=self.corr_id),
            body=message)
        while self.response is None:
            self.connection.process_data_events()
        return self.response


smessage_rpc = SsessionRpcClient()

# encrypt and send message
for i in range(4):
    print(smessage_rpc.call(
        smessage.wrap("This is test message #{}!".format(i + 1).encode('utf-8'))
    ).decode('utf-8'))
