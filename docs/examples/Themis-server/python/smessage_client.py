#!/usr/bin/python

import httplib, urllib
from pythemis import smessage
import base64
import time;

message_maker=smessage.smessage(base64.b64decode("UkVDMgAAAC1whm6SAJ7vIP18Kq5QXgLd413DMjnb6Z5jAeiRgUeekMqMC0+x"), base64.b64decode("VUVDMgAAAC1NY6NZAz62ZG25YU67a5zX1iDo0Aijz8xOu4nzE0yQhtJIQBbD"))
message = base64.b64encode(message_maker.wrap("This is test message"))
params = urllib.urlencode({'message': message})
headers = {"Content-type": "application/x-www-form-urlencoded"}
conn = httplib.HTTPSConnection("themis.cossacklabs.com")
conn.request("POST", "/api/OiJLrqNZZvUcaCH/", params, headers)
response_secure_message = conn.getresponse().read()
print(len(response_secure_message))
response_message=message_maker.unwrap(response_secure_message)
print(response_message)
conn.close()