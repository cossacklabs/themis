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
from collections import deque
from time import sleep
from threading import Thread

from pythemis import ssession
from pythemis import skeygen
from pythemis import exception

q1 = deque([])
q2 = deque([])


class SendReceiveTransport:
    def __init__(self, to_queue, from_queue):
        self.to_queue = to_queue
        self.from_queue = from_queue

    def send(self, message):
        self.to_queue.append(message)

    def receive(self):
        cont = 0
        while not self.from_queue:
            if cont >= 5:
                raise Exception("timeout")
            sleep(1)
            cont += 1
        return self.from_queue.popleft()


class Transport:
    def __init__(self, ids_pub_keys):
        self.ids_pub_keys = ids_pub_keys

    def get_pub_key_by_id(self, user_id):
        if user_id in self.ids_pub_keys:
            return self.ids_pub_keys[user_id]
        raise Exception("no such id")


class SSessionTest(unittest.TestCase):
    def setUp(self):
        server_key_generator = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        self.server_priv = server_key_generator.export_private_key()
        self.server_pub = server_key_generator.export_public_key()

        client_key_generator = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        self.client_priv = client_key_generator.export_private_key()
        self.client_pub = client_key_generator.export_public_key()
        
        self.pub_ids = {b"server": self.server_pub, b"client": self.client_pub}
        self.message = b"This is test message"

    def client(self):
        client_transport = SendReceiveTransport(q1, q2)
        session = ssession.SSession(b"client", self.client_priv,
                                    Transport(self.pub_ids))

        control_message = session.connect_request()
        while session.is_established() != 1:
            client_transport.send(control_message)
            control_message = session.unwrap(client_transport.receive())

        client_transport.send(session.wrap(self.message))
        self.assertEqual(self.message,
                         session.unwrap(client_transport.receive()))

    def server(self):
        server_transport = SendReceiveTransport(q2, q1)
        session = ssession.SSession(b"server", self.server_priv,
                                    Transport(self.pub_ids))
        while True:
            server_transport.send(session.unwrap(server_transport.receive()))
            if session.is_established() == 1:
                break
        self.assertEqual(self.message,
                         session.unwrap(server_transport.receive()))
        server_transport.send(session.wrap(self.message))

    def test_session1(self):
        client = Thread(target=self.client)
        server = Thread(target=self.server)
        client.start()
        server.start()
        client.join()
        server.join()

    def test_ssession2(self):
        keypair1 = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        keypair2 = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        user_id1 = b'user_id1'
        user_id2 = b'user_id2'

        client_session = ssession.SSession(
            user_id1, keypair1.export_private_key(),
            ssession.SimpleMemoryTransport(
                user_id2, keypair2.export_public_key()))

        server_session = ssession.SSession(
            user_id2, keypair2.export_private_key(),
            ssession.SimpleMemoryTransport(
                user_id1, keypair1.export_public_key()))

        client_data = client_session.connect_request()
        server_data = None

        while not (client_session.is_established() and
                       server_session.is_established()):
            if not server_session.is_established():
                server_data = server_session.unwrap(client_data)

            if not client_session.is_established():
                client_data = client_session.unwrap(server_data)

        data = b'some test data'

        self.assertEqual(data, client_session.unwrap(server_session.wrap(data)))
        self.assertEqual(data, server_session.unwrap(client_session.wrap(data)))

    def test_invalid_key_types(self):
        keypair2 = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        user_id2 = b'user_id2'

        transport = ssession.SimpleMemoryTransport(
            user_id2,
            keypair2.export_public_key()
        )

        keypair1 = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.EC)
        user_id1 = b'user_id1'

        self.assertIsNotNone(
            ssession.SSession(user_id1, keypair1.export_private_key(), transport),
            msg="EC private key should be accepted"
        )

        with self.assertRaises(exception.ThemisError, msg="EC public key should be rejected"):
            ssession.SSession(user_id1, keypair1.export_public_key(), transport)

        keypair1 = skeygen.GenerateKeyPair(skeygen.KEY_PAIR_TYPE.RSA)

        with self.assertRaises(exception.ThemisError, msg="RSA key should be rejected, not supported"):
            ssession.SSession(user_id1, keypair1.export_private_key(), transport)

        with self.assertRaises(exception.ThemisError, msg="RSA public key should be rejected"):
            ssession.SSession(user_id1, keypair1.export_public_key(), transport)

        with self.assertRaises(exception.ThemisError, msg="non-valid asymm key should be rejected"):
            ssession.SSession(user_id1, b'totally not a valid key', transport)

if __name__ == '__main__':
    unittest.main()
