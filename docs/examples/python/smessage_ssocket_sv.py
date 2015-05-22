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

#hadmade secure socket 
import pythemis.smessage;
import socket;

class ssocket(object):
    def __init__(self, priv_key, peer_public_key=None, socket_=None):
        if socket_==None:
            self.socket=socket.socket();
        else:
            self.socket=socket_;
        self.priv_key=priv_key;
        if peer_public_key==None:
            self.peer_public_key=None;
        else:
            self.peer_public_key=peer_public_key;
        
    def connect(self, conn_param):
        return self.socket.connect(conn_param);
    
    def sendall(self, message):
        self.socket.sendall(pythemis.smessage.ssign(self.priv_key, message)); #sign and send message

    def recv(self, buffer_length):
        return pythemis.smessage.sverify(self.peer_public_key, self.socket.recv(buffer_length)); #receive and verify received message

    def close(self):
        return self.socket.close();

    def bind(self, param):
        return self.socket.bind(param);

    def listen(self, param):
        return self.socket.listen(param);

    def accept(self):
        self.accepted, self.addr = self.socket.accept();
        return ssocket(self.priv_key, None, self.accepted), self.addr;
        
    def set_peer_pub_key(self, peer_public_key):
        self.peer_public_key = peer_public_key;
