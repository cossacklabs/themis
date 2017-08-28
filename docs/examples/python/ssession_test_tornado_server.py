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
echo server for tornado
NOTE: due to the fact that ssession has state and it is simple example, server
can handle only one session and then need restart or you can extend example and
handle many sessions per some identifier
"""

import tornado.ioloop
import tornado.web
from pythemis import ssession

client_public = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21"
server_private = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf"

session = ssession.SSession(
    b"server", server_private,
    ssession.SimpleMemoryTransport(b'client', client_public))


class MainHandler(tornado.web.RequestHandler):
    def post(self):
        print(self.request.body)
        # decrypt received message
        message = session.unwrap(self.request.body)
        if message.is_control:
            # send unwraped message to client as is if control message
            self.write(message)
        else:
            # print accepted plain message
            print(message)
            # encrypt and send reply message
            self.write(session.wrap(message))


if __name__ == "__main__":
    application = tornado.web.Application([
        (r"/", MainHandler),
    ])
    application.listen(26260)
    tornado.ioloop.IOLoop.instance().start()
