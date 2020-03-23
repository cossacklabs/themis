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
    def __init__(self, priv_key, peer_public_key=None, socket_=None):
        if socket_:
            self.socket = socket_
        else:
            self.socket = socket.socket()
        self.private_key = priv_key
        if peer_public_key:
            self.peer_public_key = peer_public_key
        else:
            self.peer_public_key = None

    def connect(self, conn_param):
        return self.socket.connect(conn_param)

    def sendall(self, message):
        # sign and send message
        self.socket.sendall(smessage.ssign(self.private_key, message))

    def recv(self, buffer_length):
        # receive and verify received message
        return smessage.sverify(self.peer_public_key,
                                self.socket.recv(buffer_length))

    def close(self):
        return self.socket.close()

    def bind(self, param):
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        return self.socket.bind(param)

    def listen(self, param):
        return self.socket.listen(param)

    def accept(self):
        self.accepted, self.addr = self.socket.accept()
        return SSocket(self.private_key, None, self.accepted), self.addr

    def set_peer_pub_key(self, peer_public_key):
        self.peer_public_key = peer_public_key
