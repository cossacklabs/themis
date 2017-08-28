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
echo client with handmade ssession wrappers (see ssession_wrappers.py) 
for none event handled transport, like plain socket
"""
import socket
import ssession_wrappers

client_private = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01"

server_public = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"


class Transport(object):
    def __init__(self):
        self.socket = socket.socket()
        self.socket.connect(("127.0.0.1", 26260))

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
        # we have only one peer with id "server"
        if user_id != b"server":
            raise Exception("no such id")
        return server_public


transport = Transport()
session = ssession_wrappers.SSessionClient(b"client", client_private,
                                           transport)
for i in range(0, 9):
    session.send(b"This is a test message")
    message = session.receive()
    print("receive: ", message)

session.send(b"finish")
