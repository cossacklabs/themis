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

import unittest
from pythemis import ssession
from pythemis import skeygen
from pythemis.exception import themis_exception

from collections import deque
from time import sleep
from threading import Thread

q1=deque([]);
q2=deque([])

class send_receive_transport:
    def __init__(self, to_queue, from_queue):
	self.toq=to_queue
	self.frq=from_queue

    def send(self, message, id):
	self.toq.append(message);

    def receive(self, id):
	cont = 0;
	while len(self.frq) == 0:
	    if cont >=5:
		raise Exception("timeout")
	    sleep(1);
	    cont+=1
	return self.frq.popleft();

class transport:
    def __init__(self, ids_pub_keys):
	self.ids_pub_keys = ids_pub_keys

    def get_pub_key_by_id(self, user_id):
	if user_id in self.ids_pub_keys:
	    return self.ids_pub_keys[user_id]
	raise Exception("no such id")

class TestSSession(unittest.TestCase):
    def setUp(self):
	server_key_generator = skeygen.themis_gen_key_pair("EC")
	self.server_priv=server_key_generator.export_private_key()
	self.server_pub=server_key_generator.export_public_key()
	client_key_generator = skeygen.themis_gen_key_pair("EC")
	self.client_priv=client_key_generator.export_private_key()
	self.client_pub=client_key_generator.export_public_key()
	
	self.pub_ids= {'server': self.server_pub, 'client': self.client_pub}
	self.message="This is test message"

    def client(self):
	client_transport=send_receive_transport(q1,q2)
        with self.assertRaises(TypeError):
	    session=ssession.ssession('client', None, transport(self.pub_ids))
        with self.assertRaises(themis_exception):
	    session=ssession.ssession('client', "", transport(self.pub_ids))
	    session.connect_request()
	session=ssession.ssession('client', self.client_priv, transport(self.pub_ids))
	control_message=session.connect_request()
	while session.is_established()!=1:
	    client_transport.send(control_message, "client_");
	    control_message=session.unwrap(client_transport.receive('client_'));
	client_transport.send(session.wrap(self.message),"client");
	self.assertEqual(self.message, session.unwrap(client_transport.receive('client')))

    def server(self):
	server_transport=send_receive_transport(q2,q1)
	session=ssession.ssession('server', self.server_priv, transport(self.pub_ids))
	while True:
	    server_transport.send(session.unwrap(server_transport.receive('server_')),"server_");
	    if session.is_established() == 1:
		break;
	self.assertEqual(self.message, session.unwrap(server_transport.receive('server')))
	server_transport.send(session.wrap(self.message), "server")

    def testSession(self):
	client = Thread(target = self.client);
	server = Thread(target = self.server);
	client.start();
	server.start();
	client.join();
	server.join();


if __name__ == '__main__':
    unittest.main()