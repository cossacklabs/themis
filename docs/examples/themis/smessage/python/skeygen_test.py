#
# Copyright (C) 2015 CossackLabs
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

#key generation example
import sys
from themis import skeygen

alg="EC";

obj = skeygen.themis_gen_key_pair(alg);
private_key = obj.export_private_key();
public_key = obj.export_public_key();

private_key_file = open(sys.argv[1],"w");
private_key_file.write(private_key);
private_key_file.close();
public_key_file = open(sys.argv[2],"w");
public_key_file.write(public_key);
public_key_file.close();
