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

import six
import warnings

from ctypes import c_int, byref, create_string_buffer, string_at

from . import themis
from .exception import ThemisError
from .exception import THEMIS_CODES


class SecureCellError(ThemisError):
    def __init__(self, message, error_code=THEMIS_CODES.INVALID_PARAMETER):
        message = 'Secure Cell: ' + message
        super(SecureCellError, self).__init__(error_code, message)


class SCellSeal(object):
    def __new__(cls, key=None, passphrase=None, **kwargs):
        """
        Make a new Secure Cell in Seal mode.

        You must specify either key= or passphrase= keyword argument.

        :type key: bytes
        :type passphrase: Union(str, bytes)
        """
        if key is None and passphrase is None:
            raise SecureCellError('missing key or passphrase')
        if key is not None and passphrase is not None:
            raise SecureCellError('key and passphrase cannot be specified at the same time')
        if key is not None:
            return object.__new__(SCellSeal)
        if passphrase is not None:
            return object.__new__(SCellSealPassphrase)

    def __init__(self, key):
        """
        Make a new Secure Cell in Seal mode with a master key.

        :type key: bytes
        """
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed creating")
        if not isinstance(key, six.binary_type):
            warnings.warn('master key should be "bytes", '
                          'consider using "passphrase=" API with strings')
        self.key = key

    def encrypt(self, message, context=None):
        context_length = len(context) if context else 0
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(encrypted_message_length)) != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), encrypted_message,
                byref(encrypted_message_length)) != THEMIS_CODES.SUCCESS:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed encrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context=None):
        context_length = len(context) if context else 0
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), None,
                byref(decrypted_message_length)) != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_seal(
                self.key, len(self.key), context, context_length,
                message, len(message), decrypted_message,
                byref(decrypted_message_length)) != THEMIS_CODES.SUCCESS:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Seal) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class SCellSealPassphrase(SCellSeal):
    def __new__(cls, passphrase, **kwargs):
        """
        Make a new Secure Cell in Seal mode with a passphrase.

        :type passphrase: Union(str, bytes)
        """
        return object.__new__(SCellSealPassphrase)

    def __init__(self, passphrase, encoding='utf-8'):
        """
        Make a new Secure Cell in Seal mode with a passphrase.

        :type passphrase: Union(str, bytes)
        """
        if not passphrase:
            raise SecureCellError('passphrase cannot be empty')
        if isinstance(passphrase, six.text_type):
            passphrase = passphrase.encode(encoding)
        elif isinstance(passphrase, six.binary_type):
            pass
        else:
            raise SecureCellError('passphrase must be either "unicode" or "bytes"')
        self.passphrase = passphrase

    def encrypt(self, message, context=None):
        """
        Encrypt given message with optional context.

        :type message: bytes
        :type context: bytes
        :returns bytes
        """
        context_length = len(context) if context else 0
        encrypted_message_length = c_int(0)

        res = themis.themis_secure_cell_encrypt_seal_with_passphrase(
                self.passphrase, len(self.passphrase),
                context, context_length,
                message, len(message),
                None, byref(encrypted_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise SecureCellError("encryption failed", error_code=res)

        encrypted_message = create_string_buffer(encrypted_message_length.value)
        res = themis.themis_secure_cell_encrypt_seal_with_passphrase(
                self.passphrase, len(self.passphrase),
                context, context_length,
                message, len(message),
                encrypted_message, byref(encrypted_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise SecureCellError("encryption failed", error_code=res)

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context=None):
        """
        Decrypt given message with optional context.

        :type message: bytes
        :type context: bytes
        :returns bytes
        """
        context_length = len(context) if context else 0
        decrypted_message_length = c_int(0)

        res = themis.themis_secure_cell_decrypt_seal_with_passphrase(
                self.passphrase, len(self.passphrase),
                context, context_length,
                message, len(message),
                None, byref(decrypted_message_length))
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise SecureCellError("decryption failed", error_code=res)

        decrypted_message = create_string_buffer(decrypted_message_length.value)
        res = themis.themis_secure_cell_decrypt_seal_with_passphrase(
                self.passphrase, len(self.passphrase),
                context, context_length,
                message, len(message),
                decrypted_message, byref(decrypted_message_length))
        if res != THEMIS_CODES.SUCCESS:
            raise SecureCellError("decryption failed", error_code=res)

        return string_at(decrypted_message, decrypted_message_length.value)


class SCellTokenProtect(object):
    def __init__(self, key):
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed creating")
        if not isinstance(key, six.binary_type):
            warnings.warn('master key should be "bytes", '
                          'consider using skeygen.GenerateSymmetricKey()')
        self.key = key

    def encrypt(self, message, context=None):
        context_length_ = len(context) if context else 0
        encrypted_message_length = c_int(0)
        context_length = c_int(0)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context, context_length_,
                message, len(message), None, byref(context_length), None,
                byref(encrypted_message_length)) != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        token = create_string_buffer(context_length.value)
        if themis.themis_secure_cell_encrypt_token_protect(
                self.key, len(self.key), context, context_length_,
                message, len(message), token, byref(context_length),
                encrypted_message, byref(encrypted_message_length)) != THEMIS_CODES.SUCCESS:
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
        if res != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_token_protect(
                self.key, len(self.key), context, context_length,
                message, len(message), token, len(token), decrypted_message,
                byref(decrypted_message_length)) != THEMIS_CODES.SUCCESS:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Token Protect) failed decrypting")

        return string_at(decrypted_message, decrypted_message_length.value)


class SCellContextImprint(object):
    def __init__(self, key):
        if not key:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed creating")
        if not isinstance(key, six.binary_type):
            warnings.warn('master key should be "bytes", '
                          'consider using skeygen.GenerateSymmetricKey()')
        self.key = key

    def encrypt(self, message, context):
        encrypted_message_length = c_int(0)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(encrypted_message_length)) != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed encrypting")
        encrypted_message = create_string_buffer(encrypted_message_length.value)
        if themis.themis_secure_cell_encrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), encrypted_message,
                byref(encrypted_message_length)) != THEMIS_CODES.SUCCESS:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed encrypting")

        return string_at(encrypted_message, encrypted_message_length.value)

    def decrypt(self, message, context):
        decrypted_message_length = c_int(0)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), None,
                byref(decrypted_message_length)) != THEMIS_CODES.BUFFER_TOO_SMALL:
            raise ThemisError(THEMIS_CODES.FAIL,
                              "Secure Cell (Context Imprint) failed decrypting")
        decrypted_message = create_string_buffer(decrypted_message_length.value)
        if themis.themis_secure_cell_decrypt_context_imprint(
                self.key, len(self.key), message, len(message),
                context, len(context), decrypted_message,
                byref(decrypted_message_length)) != THEMIS_CODES.SUCCESS:
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
