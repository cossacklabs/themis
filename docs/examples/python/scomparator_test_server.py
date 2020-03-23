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
"""

import socket
from pythemis import scomparator

server_socket = socket.socket()
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind(("127.0.0.1", 26260))
try:
    server_socket.listen(1)
    connection, addr = server_socket.accept()
    try:
        comparator = scomparator.SComparator(b"shared secret")
        while not comparator.is_compared():
            data = comparator.proceed_compare(connection.recv(1024))
            connection.sendall(data)

        if comparator.is_equal():
            print("match")
        else:
            print("not match")
    finally:
        connection.close()
finally:
    server_socket.close()
