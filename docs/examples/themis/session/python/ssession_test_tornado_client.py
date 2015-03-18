from themis import ssession;
#import ssession_mem_transport;
import tornado.ioloop
import tornado.httpclient;

client_priv = str('\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01');

server_pub  = str('\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0');

class transport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):
        if user_id != "server":
            raise Exception("no such id");
        return server_pub; 

http_client = tornado.httpclient.HTTPClient();
session=ssession.ssession("client", client_priv, transport());
try:
    response = http_client.fetch(tornado.httpclient.HTTPRequest("http://127.0.0.1:26260", "POST", None, session.connect_request()));
    res, message=session.unwrap(response.body);
    while res==1:
        response = http_client.fetch(tornado.httpclient.HTTPRequest("http://127.0.0.1:26260", "POST", None, message));
        res, message = session.unwrap(response.body);

    response = http_client.fetch(tornado.httpclient.HTTPRequest("http://127.0.0.1:26260", "POST", None, session.wrap("This is test message")));
    res, message = session.unwrap(response.body);
    print message;
    
except tornado.httpclient.HTTPError as e:
    print("Error: " + str(e));
except Exception as e:
    print("Error: " + str(e));
http_client.close();
