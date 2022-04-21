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
from ctypes import create_string_buffer, c_int, string_at, byref
from enum import IntEnum

from . import themis
from .exception import ThemisError, THEMIS_CODES


class SMessage(object):
    def __init__(self, private_key, peer_public_key):
        if not private_key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Message: missing private key")
        if not peer_public_key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Message: missing public key")
        if not _private_key(private_key):
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Message: invalid private key")
        if not _public_key(peer_public_key):
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Message: invalid public key")
        self.private_key = private_key
        self.peer_public_key = peer_public_key

    def wrap(self, message):
        encrypted_message_length = c_int(0)
        res = themis.themis_secure_message_encrypt(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), None, byref(encrypted_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(res, "Secure Message failed to encrypt")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        res = themis.themis_secure_message_encrypt(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), encrypted_message,
            byref(encrypted_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise ThemisError(res, "Secure Message failed to encrypt")

        return string_at(encrypted_message, encrypted_message_length.value)

    def unwrap(self, message):
        plain_message_length = c_int(0)
        res = themis.themis_secure_message_decrypt(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), None, byref(plain_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(res, "Secure Message failed to decrypt")
        plain_message = create_string_buffer(plain_message_length.value)
        res = themis.themis_secure_message_decrypt(
            self.private_key, len(self.private_key),
            self.peer_public_key, len(self.peer_public_key),
            message, len(message), plain_message, byref(plain_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise ThemisError(res, "Secure Message failed to decrypt")
        return string_at(plain_message, plain_message_length.value)


def ssign(private_key, message):
    if not private_key:
        raise ThemisError(THEMIS_CODES.FAIL,
                          "Secure Message: missing private key")
    if not _private_key(private_key):
        raise ThemisError(THEMIS_CODES.FAIL,
                          "Secure Message: invalid private key")
    encrypted_message_length = c_int(0)
    res = themis.themis_secure_message_sign(
        private_key, len(private_key), message, len(message),
        None, byref(encrypted_message_length))
    if res != THEMIS_CODES.BUFFER_TOO_SMALL:
        raise ThemisError(res, "Secure Message failed to sign")
    encrypted_message = create_string_buffer(encrypted_message_length.value)
    res = themis.themis_secure_message_sign(
        private_key, len(private_key), message, len(message),
        encrypted_message, byref(encrypted_message_length))
    if res != THEMIS_CODES.SUCCESS:
        raise ThemisError(res, "Secure Message failed to sign")
    return string_at(encrypted_message, encrypted_message_length.value)


def sverify(public_key, message):
    if not public_key:
        raise ThemisError(THEMIS_CODES.FAIL,
                          "Secure Message: missing public key")
    if not _public_key(public_key):
        raise ThemisError(THEMIS_CODES.FAIL,
                          "Secure Message: invalid public key")
    plain_message_length = c_int(0)
    res = themis.themis_secure_message_verify(
        public_key, len(public_key), message, len(message),
        None, byref(plain_message_length))
    if res != THEMIS_CODES.BUFFER_TOO_SMALL:
        raise ThemisError(res, "Secure Message failed to verify")
    plain_message = create_string_buffer(plain_message_length.value)
    res = themis.themis_secure_message_verify(
        public_key, len(public_key), message, len(message),
        plain_message, byref(plain_message_length))
    if res != THEMIS_CODES.SUCCESS:
        raise ThemisError(res, "Secure Message failed to verify")

    return string_at(plain_message, plain_message_length.value)


class THEMIS_KEY(IntEnum):
    INVALID = 0
    RSA_PRIVATE = 1
    RSA_PUBLIC = 2
    EC_PRIVATE = 3
    EC_PUBLIC = 4


def _public_key(key):
    res = themis.themis_is_valid_asym_key(key, len(key))
    if res != THEMIS_CODES.SUCCESS:
        return False
    kind = themis.themis_get_asym_key_kind(key, len(key))
    return kind in [THEMIS_KEY.RSA_PUBLIC, THEMIS_KEY.EC_PUBLIC]


def _private_key(key):
    res = themis.themis_is_valid_asym_key(key, len(key))
    if res != THEMIS_CODES.SUCCESS:
        return False
    kind = themis.themis_get_asym_key_kind(key, len(key))
    return kind in [THEMIS_KEY.RSA_PRIVATE, THEMIS_KEY.EC_PRIVATE]


class smessage(SMessage):
    def __init__(self, *args, **kwargs):
        warnings.warn("smessage is deprecated in favor of SMessage.")
        super(smessage, self).__init__(*args, **kwargs)
