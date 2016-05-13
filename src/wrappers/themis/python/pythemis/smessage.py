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

from ctypes import cdll, create_string_buffer, c_int, string_at, byref
from ctypes.util import find_library

from .exception import themis_exception
from .exception import THEMIS_CODES

themis = cdll.LoadLibrary(find_library('themis'))


class smessage(object):
    def __init__(self, private_key_, peer_public_key_):
        if len(private_key_) == 0 or len(peer_public_key_) == 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Message failed creating")
        self.private_key = private_key_
        self.peer_public_key = peer_public_key_

    def wrap(self, message):
        encrypted_message_length=c_int(0)
        res = themis.themis_secure_message_wrap(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), None, byref(encrypted_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise themis_exception(res, "Secure Message failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        res = themis.themis_secure_message_wrap(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), encrypted_message,
            byref(encrypted_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise themis_exception(res, "Secure Message failed encrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def unwrap(self,message):
        plain_message_length = c_int(0)
        res = themis.themis_secure_message_unwrap(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), None, byref(plain_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise themis_exception(res,
                                   "Secure Message failed decrypting")
        plain_message = create_string_buffer(plain_message_length.value)
        res = themis.themis_secure_message_unwrap(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), plain_message, byref(plain_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise themis_exception(
                res, "Secure Message failed decrypting")
        return string_at(plain_message, plain_message_length.value)

def ssign(private_key, message):
        encrypted_message_length = c_int(0)
        res = themis.themis_secure_message_wrap(
            private_key, len(private_key), None, 0, message, len(message),
            None, byref(encrypted_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise themis_exception(
                res, "Secure Message failed singing")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        res = themis.themis_secure_message_wrap(
            private_key, len(private_key), None, 0, message, len(message),
            encrypted_message, byref(encrypted_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise themis_exception(
                res, "Secure Message failed singing")
        return string_at(encrypted_message, encrypted_message_length.value)

def sverify(public_key, message):
        plain_message_length = c_int(0)
        res = themis.themis_secure_message_unwrap(
            None, 0, public_key, len(public_key), message, len(message),
            None, byref(plain_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise themis_exception(
                res,  "Secure Message failed verifying")
        plain_message = create_string_buffer(plain_message_length.value)
        res = themis.themis_secure_message_unwrap(
            None, 0, public_key, len(public_key), message, len(message),
            plain_message, byref(plain_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise themis_exception(
                res, "Secure Message failed verifying")

        return string_at(plain_message, plain_message_length.value)
