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

#ssession wrappers for non event handled transport (like olain socket)
from pythemis import ssession;

class ssession_server(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=ssession.ssession(user_id, sign_key, transport);
        while self.session.is_established()!=True:         #1. establish session
            self.session.receive();                           #2. establish session

    def receive(self):
        return self.session.receive();                        #receive message

    def send(self, message):
        self.session.send(message);                        #send message



class ssession_client(object):
    def __init__(self, user_id, sign_key, transport):
        self.session=ssession.ssession(user_id, sign_key, transport);
        self.session.connect();                                #1. establish session
        while self.session.is_established()!=True:        #2  establish session
            self.session.receive();                        #3. establish session

    def receive(self):
        return self.session.receive();                        #receive message

    def send(self, message):
        self.session.send(message);                        #send message
