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

from ctypes import cdll, c_int, create_string_buffer, byref, string_at
from ctypes.util import find_library

from .exception import themis_exception, THEMIS_CODES

themis = cdll.LoadLibrary(find_library('themis'))


class themis_gen_key_pair(object):
    def __init__(self, alg):
        self.private_key_length = c_int(4096)
        self.public_key_length = c_int(4096)
        self.private_key = create_string_buffer(self.private_key_length.value)
        self.public_key = create_string_buffer(self.public_key_length.value)
        if alg == "EC":
            if themis.themis_gen_ec_key_pair(
                    self.private_key, byref(self.private_key_length),
                    self.public_key, byref(self.public_key_length)) != 0:
                raise themis_exception(THEMIS_CODES.FAIL,
                                       "Themis failed generating EC KeyPair")
        elif alg == "RSA":
            if themis.themis_gen_rsa_key_pair(
                    self.private_key, byref(self.private_key_length),
                    self.public_key, byref(self.public_key_length)) != 0:
                raise themis_exception(THEMIS_CODES.FAIL,
                                       "Themis failed generating RSA KeyPair")

    def export_private_key(self):
        return string_at(self.private_key, self.private_key_length.value)

    def export_public_key(self):
        return string_at(self.public_key, self.public_key_length.value)
