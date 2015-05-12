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

#echo server with handmade ssession wrappers (see ssession_wrappers.py) 
#for none event handled transport, like plain socket
import ssession_wrappers;
import socket;
import ctypes;

client_pub = str('\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21');

server_priv= str('\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf');

class transport(object):			#callbacks object
    def __init__(self, socket):
        self.socket=socket;

    def __del__(self):
        self.socket.close();
        
    def send(self, message):			#send callback
	a=1;
        self.socket.sendall(message);

    def receive(self, buffer_length):		#receive callback
        return self.socket.recv(buffer_length);

    def get_pub_key_by_id(self, user_id):	#necessary callback
        if user_id != "client":			#we have only one peer with id "client"
            raise Exception("no such id");
        return client_pub; 
        
    
conn = socket.socket();
conn.bind(("127.0.0.1", 26260));
conn.listen(1);
accepted, addr = conn.accept();
transport_=transport(accepted);

session=ssession_wrappers.ssession_server("server", server_priv, transport_);
while True:
    message = session.receive();
    print "receive:", message;
    if message == "finish":
	break;
    session.send(message);

accepted.close();
conn.close();
