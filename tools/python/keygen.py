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

import sys
import os
from pythemis.skeygen import KEY_PAIR_TYPE, GenerateKeyPair

args_count = len(sys.argv)
if args_count != 3 and args_count != 1:
    print("Usage: python {} <private_key_path> <public_key_path>".format(
        os.path.basename(__file__)))
    exit(1)

keypair = GenerateKeyPair(KEY_PAIR_TYPE.EC)
private_key = keypair.export_private_key()
public_key = keypair.export_public_key()
if args_count == 1:
    private_key_path = 'key'
    public_key_path = 'key.pub'
elif args_count == 3:
    private_key_path = sys.argv[1]
    public_key_path = sys.argv[2]


with open(private_key_path, "wb") as private_key_file:
    private_key_file.write(private_key)

os.chmod(private_key_path, 0o400)

with open(public_key_path, "wb") as public_key_file:
    public_key_file.write(public_key)