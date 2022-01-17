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
import warnings
from ctypes import c_int, create_string_buffer, byref, string_at

from . import themis
from .exception import ThemisError, THEMIS_CODES


class KEY_PAIR_TYPE(object):
    EC = 'EC'
    RSA = 'RSA'
    CHOICES = (EC, RSA)


class GenerateKeyPair(object):
    def __init__(self, alg):
        self.private_key_length = c_int(4096)
        self.public_key_length = c_int(4096)
        self.private_key = create_string_buffer(self.private_key_length.value)
        self.public_key = create_string_buffer(self.public_key_length.value)

        if alg not in KEY_PAIR_TYPE.CHOICES:
            raise ThemisError(THEMIS_CODES.FAIL, "Incorrect KeyPair type")

        if alg == KEY_PAIR_TYPE.EC:
            if themis.themis_gen_ec_key_pair(
                    self.private_key, byref(self.private_key_length),
                    self.public_key, byref(self.public_key_length)) != 0:
                raise ThemisError(THEMIS_CODES.FAIL,
                                  "Themis failed generating EC KeyPair")
        elif alg == KEY_PAIR_TYPE.RSA:
            if themis.themis_gen_rsa_key_pair(
                    self.private_key, byref(self.private_key_length),
                    self.public_key, byref(self.public_key_length)) != 0:
                raise ThemisError(THEMIS_CODES.FAIL,
                                  "Themis failed generating RSA KeyPair")

    def export_private_key(self):
        return string_at(self.private_key, self.private_key_length.value)

    def export_public_key(self):
        return string_at(self.public_key, self.public_key_length.value)


class themis_gen_key_pair(GenerateKeyPair):
    def __init__(self, *args, **kwargs):
        warnings.warn("themis_gen_key_pair is deprecated in favor of "
                      "GenerateKeyPair.")
        super(themis_gen_key_pair, self).__init__(*args, **kwargs)


def GenerateSymmetricKey():
    """Returns a byte string with newly generated key."""
    key_length = c_int(0)
    res = themis.themis_gen_sym_key(None, byref(key_length))
    if res != THEMIS_CODES.BUFFER_TOO_SMALL:
        raise ThemisError(res, "Themis failed to get symmetric key size")

    key = create_string_buffer(key_length.value)
    res = themis.themis_gen_sym_key(key, byref(key_length))
    if res != THEMIS_CODES.SUCCESS:
        raise ThemisError(res, "Themis failed to generate symmetric key")

    return string_at(key, key_length.value)
