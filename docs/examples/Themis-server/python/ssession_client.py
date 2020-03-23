#!/usr/bin/python

import httplib, urllib
from pythemis import ssession
import base64
import time;


class transport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):        #necessary callback function
        if user_id != b"nXQpVsglEGFJgfK":                        #we have only one peer with id "server"
            raise Exception("no such id");
        return base64.b64decode("VUVDMgAAAC2yfQpBAzarlWrwxjlDduEH6Wdfah46bDVLGGCAqYYkVS+dlrcV");

session=ssession.ssession(b"gounDbllopcCcVB",base64.b64decode("UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x"), transport())
data = session.connect_request()
params = urllib.urlencode({'message': base64.b64encode(data)})
headers = {"Content-type": "application/x-www-form-urlencoded"}
conn = httplib.HTTPSConnection("themis.cossacklabs.com")
print(params)
conn.request("POST", "/api/gounDbllopcCcVB/", params, headers)
resp= conn.getresponse()
print(resp.status)
response_msg = session.unwrap(resp.read())
while response_msg.is_control:
    params = urllib.urlencode({'message': base64.b64encode(response_msg)})
    print(params)
    conn.request("POST", "/api/gounDbllopcCcVB/", params, headers)
    resp= conn.getresponse()
    print(resp.status)
    response_msg = session.unwrap(resp.read())

params = urllib.urlencode({'message': base64.b64encode(session.wrap(b"Hello Themis CI server"))})
conn.request("POST", "/api/gounDbllopcCcVB/", params, headers)
response_msg = session.unwrap(conn.getresponse().read())
print(response_msg)

conn.close()