#
# Copyright (c) 2017 Cossack Labs Limited
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

import sys
from base64 import b64encode, b64decode
from pythemis import smessage

_, COMMAND, SENDER_PRIVATE_KEY, RECIPIENT_PUBLIC_KEY, MESSAGE = range(5)

if len(sys.argv) != 5:
    print('Usage: <command: enc | dec | sign | verify > <send_private_key> <recipient_public_key> <message>')
    exit(1)

command = sys.argv[COMMAND]
private_key_path = sys.argv[SENDER_PRIVATE_KEY]
public_key_path = sys.argv[RECIPIENT_PUBLIC_KEY]
message = sys.argv[MESSAGE]

with open(private_key_path, 'rb') as f:
    private_key = f.read()

with open(public_key_path, 'rb') as f:
    public_key = f.read()

message_encrypter = smessage.SMessage(private_key, public_key)

if command == 'enc':
    encrypted = message_encrypter.wrap(message.encode('utf-8'))
    encoded = b64encode(encrypted)
    print(encoded.decode('ascii'))
elif command == 'dec':
    decoded = b64decode(message.encode('utf-8'))
    decrypted = message_encrypter.unwrap(decoded)
    print(decrypted.decode('utf-8'))
elif command == 'sign':
    encrypted = smessage.ssign(private_key, message.encode('utf-8'))
    encoded = b64encode(encrypted)
    print(encoded.decode('ascii'))
elif command == 'verify':
    decoded = b64decode(message.encode('utf-8'))
    decrypted = smessage.sverify(public_key, decoded)
    print(decrypted.decode('utf-8'))
else:
    print('Wrong command, use <enc | dev | sign | verify>')
    exit(1)
