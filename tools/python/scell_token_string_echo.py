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
from pythemis import scell

_, COMMAND, KEY, MESSAGE, CONTEXT = range(5)

if len(sys.argv) not in (4, 5):
    print('Usage: <command: enc | dec > <key> <message> <context (optional)>')
    exit(1)

command = sys.argv[COMMAND]
key = sys.argv[KEY]
context = sys.argv[CONTEXT] if len(sys.argv) == 5 else ''

cell = scell.SCellTokenProtect(key.encode('utf-8'))
if command == 'enc':
    message = sys.argv[MESSAGE]
    encrypted, token = cell.encrypt(
        message.encode('utf-8'), context.encode('utf-8'))
    encoded_message = b64encode(encrypted).decode('ascii')
    encoded_token = b64encode(token).decode('ascii')
    print(','.join((encoded_message, encoded_token)))
elif command == 'dec':
    message, token = sys.argv[MESSAGE].split(',')
    decoded_message = b64decode(message.encode('utf-8'))
    decoded_token = b64decode(token.encode('utf-8'))
    decrypted = cell.decrypt(
        decoded_message, decoded_token, context.encode('utf-8'))
    print(decrypted.decode('utf-8'))
else:
    print('Wrong command, use "enc" or "dec"')
    exit(1)
