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

"""
ssession wrappers for non event handled transport (like plain socket)
NOTE: due to the fact that ssession has state and it is simple example, server
can handle only one session and then need restart or you can extend example and
handle many sessions per some identifier
"""

from pythemis import ssession


class SSessionServer(object):
    def __init__(self, user_id, sign_key, transport):
        self.session = ssession.SSession(user_id, sign_key, transport)
        # 1. establish session
        while not self.session.is_established():
            self.session.receive()

    def receive(self):
        return self.session.receive()

    def send(self, message):
        self.session.send(message)


class SSessionClient(object):
    def __init__(self, user_id, sign_key, transport):
        self.session = ssession.SSession(user_id, sign_key, transport)
        # 1. establish session
        self.session.connect()
        while not self.session.is_established():
            self.session.receive()

    def receive(self):
        return self.session.receive()

    def send(self, message):
        self.session.send(message)
