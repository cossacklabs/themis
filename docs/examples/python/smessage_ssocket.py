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

import socket
from pythemis import smessage


class SSocket(object):
    def __init__(self, private_key, peer_public_key=None, socket_=None):
        if socket_:
            self.socket = socket_
        else:
            self.socket = socket.socket()
        self.priv_key = private_key
        if peer_public_key:
            self.smessage = smessage.SMessage(private_key, peer_public_key)
        else:
            self.message = None

    def connect(self, connection_params):
        return self.socket.connect(connection_params)

    def sendall(self, message):
        # encrypt and send message
        self.socket.sendall(self.smessage.wrap(message))

    def recv(self, buffer_length):
        data = self.socket.recv(buffer_length)
        # decrypt received message
        return self.smessage.unwrap(data)

    def close(self):
        return self.socket.close()

    def bind(self, param):
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        return self.socket.bind(param)

    def listen(self, param):
        return self.socket.listen(param)

    def accept(self):
        self.accepted, self.addr = self.socket.accept()
        return SSocket(self.priv_key, None, self.accepted), self.addr

    def set_peer_pub_key(self, peer_public_key):
        self.smessage = smessage.SMessage(self.priv_key, peer_public_key)
