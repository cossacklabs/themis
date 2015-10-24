#!/usr/bin/python

import httplib, urllib
from pythemis import smessage
import base64
import time;

message_maker=smessage.smessage(base64.b64decode("UkVDMgAAAC3219ClAMr8iP+6YQw8owIaRPnZKQsV4Ch9OWbXJZ2jeeJQIpO6"), base64.b64decode("VUVDMgAAAC3OHpItAicSfMWzdsTs0W5saoV/sgCBWXP2qIsGStq9r2Y4b0Jr"))
message = base64.b64encode(message_maker.wrap("Hello Themis CI server"))
params = urllib.urlencode({'message': message})
headers = {"Content-type": "application/x-www-form-urlencoded"}
conn = httplib.HTTPSConnection("themis.cossacklabs.com")
conn.request("POST", "/api/moxxyBpVqFouTPa/", params, headers)
response_secure_message = conn.getresponse().read()
response_message=message_maker.unwrap(response_secure_message)
print(response_message)
conn.close()