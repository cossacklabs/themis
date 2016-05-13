# coding: utf-8
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

from ctypes import cdll, c_int, byref, create_string_buffer, string_at
from ctypes.util import find_library

from .exception import themis_exception
from .exception import THEMIS_CODES


themis = cdll.LoadLibrary(find_library('themis'))


class scell_seal(object):
    def __init__(self, key_):
        if len(key_) == 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Seal) failed creating")
        self.key = key_

    def encrypt(self, message, context=None):
        if context is None:
            context_length = 0
        else:
            context_length = len(context)
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(encrypted_message_length)) != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Seal) failed ecrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), encrypted_message,
                byref(encrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Seal) failed ecrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context=None):
        if context is None:
            context_length = 0
        else:
            context_length = len(context)
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(decrypted_message_length)) != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Seal) failed derypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Seal) failed derypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class scell_token_protect(object):
    def __init__(self, key_):
        if len(key_) == 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Token Protect) failed creating")
        self.key = key_

    def encrypt(self, message, context_=None):
        if context_ is None:
            context_length_ = 0
        else:
            context_length_ = len(context_)
        encrypted_message_length = c_int(0)
        context_length = c_int(0)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context_, context_length_,
                message, len(message), None, byref(context_length), None,
                byref(encrypted_message_length)) != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Token Protect) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        context = create_string_buffer(context_length.value)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context_, context_length_,
                message, len(message), context, byref(context_length),
                encrypted_message, byref(encrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Token Protect) failed encrypting")
        return (string_at(encrypted_message, encrypted_message_length.value),
                string_at(context, context_length))

    def decrypt(self, message, context, context_=None):
        if context_ is None:
            context_length_ = 0
        else:
            context_length_ = len(context_)
        decrypted_message_length = c_int(0)
        res = themis.themis_secure_cell_decrypt_token_protect(
            self.key, len(self.key), context_, context_length_,
            message, len(message), context, len(context), None,
            byref(decrypted_message_length))
        if res != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Token Protect) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_token_protect(
                self.key, len(self.key), context_, context_length_,
                message, len(message), context, len(context), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Token Protect) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class scell_context_imprint(object):
    def __init__(self, key_):
        if len(key_) == 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Context Imprint) failed creating")
        self.key = key_

    def encrypt(self, message, context):
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(encrypted_message_length)) != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Context Imprint) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), encrypted_message,
                byref(encrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Context Imprint) failed encrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context):
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(decrypted_message_length)) != -4:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Context Imprint) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise themis_exception(THEMIS_CODES.FAIL,
                                   "Secure Cell (Context Imprint) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)
