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
import warnings
from ctypes import cdll, c_int, byref, create_string_buffer, string_at
from ctypes.util import find_library

from .exception import ThemisError
from .exception import THEMIS_CODES

themis = cdll.LoadLibrary(find_library('themis'))


class SCellSeal(object):
    def __init__(self, key):
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed creating")
        self.key = key

    def encrypt(self, message, context=None):
        context_length = len(context) if context else 0
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(encrypted_message_length)) != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed ecrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), encrypted_message,
                byref(encrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed ecrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context=None):
        context_length = len(context) if context else 0
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(decrypted_message_length)) != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed derypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed derypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class SCellTokenProtect(object):
    def __init__(self, key):
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed creating")
        self.key = key

    def encrypt(self, message, context=None):
        context_length_ = len(context) if context else 0
        encrypted_message_length = c_int(0)
        context_length = c_int(0)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context, context_length_,
                message, len(message), None, byref(context_length), None,
                byref(encrypted_message_length)) != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        token = create_string_buffer(context_length.value)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context, context_length_,
                message, len(message), token, byref(context_length),
                encrypted_message, byref(encrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed encrypting")
        return (string_at(encrypted_message, encrypted_message_length.value),
                string_at(token, context_length))

    def decrypt(self, message, token, context=None):
        context_length = len(context) if context else 0
        decrypted_message_length = c_int(0)
        res = themis.themis_secure_cell_decrypt_token_protect(
            self.key, len(self.key), context, context_length,
            message, len(message), token, len(token), None,
            byref(decrypted_message_length))
        if res != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_token_protect(
                self.key, len(self.key), context, context_length,
                message, len(message), token, len(token), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class SCellContextImprint(object):
    def __init__(self, key):
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed creating")
        self.key = key

    def encrypt(self, message, context):
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(encrypted_message_length)) != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), encrypted_message,
                byref(encrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed encrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context):
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(decrypted_message_length)) != -4:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), decrypted_message,
                byref(decrypted_message_length)) != 0:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class scell_seal(SCellSeal):
    def __init__(self, *args, **kwargs):
        warnings.warn("scell_seal is deprecated in favor of SCellSeal.")
        super(scell_seal, self).__init__(*args, **kwargs)


class scell_token_protect(SCellTokenProtect):
    def __init__(self, *args, **kwargs):
        warnings.warn("scell_token_protect is deprecated in favor of "
                      "SCellTokenProtect.")
        super(scell_token_protect, self).__init__(*args, **kwargs)


class scell_context_imprint(SCellContextImprint):
    def __init__(self, *args, **kwargs):
        warnings.warn("scell_context_imprint is deprecated in favor of "
                      "SCellContextImprint.")
        super(scell_context_imprint, self).__init__(*args, **kwargs)
