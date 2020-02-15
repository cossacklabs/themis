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
"""
echo server with handmade ssession wrappers (see ssession_wrappers.py) 
for none event handled transport, like plain socket

NOTE: due to the fact that ssession has state and it is simple example, server
can handle only one session and then need restart or you can extend example and
handle many sessions per some identifier
"""

import socket
import ssession_wrappers

client_public = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21"
server_private = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf"


class Transport(object):
    def __init__(self, socket_):
        self.socket = socket_

    def __del__(self):
        self.socket.close()

    # send callback
    def send(self, message):
        self.socket.sendall(message)

    # receive callback
    def receive(self, buffer_length):
        return self.socket.recv(buffer_length)

    # necessary callback
    def get_pub_key_by_id(self, user_id):
        # we have only one peer with id "client"
        if user_id != b"client":
            raise Exception("no such id")
        return client_public


conn = socket.socket()
conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
conn.bind(("127.0.0.1", 26260))
conn.listen(1)
accepted, addr = conn.accept()
transport = Transport(accepted)

session = ssession_wrappers.SSessionServer(b"server", server_private,
                                           transport)
while True:
    message = session.receive()
    print("receive:", message)
    if message == b"finish":
        break
    session.send(message)

accepted.close()
conn.close()
