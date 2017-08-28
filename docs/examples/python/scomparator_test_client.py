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
from pythemis import scomparator

comparator = scomparator.SComparator(b"shared secret")
socket = socket.socket()
socket.connect(("127.0.0.1", 26260))
try:
    data = comparator.begin_compare()

    while not comparator.is_compared():
        socket.sendall(data)
        data = comparator.proceed_compare(socket.recv(1024))

    if comparator.is_equal():
        print("match")
    else:
        print("not match")
finally:
    socket.close()
