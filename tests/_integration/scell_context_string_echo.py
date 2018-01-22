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

if len(sys.argv) != 5:
    print('Usage: <command: enc | dec > <key> <message> <context>')
    exit(1)

command = sys.argv[COMMAND]
key = sys.argv[KEY]
message = sys.argv[MESSAGE]
context = sys.argv[CONTEXT]
cell = scell.SCellContextImprint(key.encode('utf-8'))
if command == 'enc':
    encrypted = cell.encrypt(message.encode('utf-8'), context.encode('utf-8'))
    encoded = b64encode(encrypted)
    print(encoded.decode('ascii'))
elif command == 'dec':
    decoded = b64decode(message.encode('utf-8'))
    decrypted = cell.decrypt(decoded, context.encode('utf-8'))
    print(decrypted.decode('utf-8'))
else:
    print('Wrong command, use "enc" or "dec"')
    exit(1)
