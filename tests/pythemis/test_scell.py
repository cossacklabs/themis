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

import unittest
import warnings

from pythemis.scell import SCellSeal, SCellSealPassphrase
from pythemis.scell import SCellTokenProtect, SCellContextImprint
from pythemis.skeygen import GenerateSymmetricKey
from pythemis.exception import ThemisError


class BaseSCellTestMixin(unittest.TestCase):
    def setUp(self):
        self.context = b'some context'
        self.incorrect_context = b'another context'
        self.passphrase = u'pretty please with a cherry on top'
        self.incorrect_passphrase = u'this passphrase is incorrect'
        self.key = GenerateSymmetricKey()
        self.incorrect_key = GenerateSymmetricKey()
        self.data = b'some data'
        # zero length
        self.incorrect_data = b''
        super(BaseSCellTestMixin, self).setUp()


class SCellSealMasterKeyTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellSeal(None)
        with self.assertRaises(ThemisError):
            SCellSeal(b'')
        with warnings.catch_warnings(record=True) as w:
            SCellSeal(u'passphrase')
            self.assertEqual(len(w), 1)
            self.assertTrue('master key should be "bytes"' in str(w[-1].message))

    def test_encrypt_decrypt(self):
        scell1 = SCellSeal(self.key)
        scell2 = SCellSeal(self.key)
        scell3 = SCellSeal(self.incorrect_key)
        self.assertEqual(self.data, scell2.decrypt(scell1.encrypt(self.data)))
        with self.assertRaises(ThemisError):
            scell3.decrypt(scell1.encrypt(self.data))

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data)

    def test_encrypt_decrypt_context(self):
        scell1 = SCellSeal(self.key)
        scell2 = SCellSeal(self.key)
        scell3 = SCellSeal(self.incorrect_key)
        self.assertEqual(
            self.data,
            scell2.decrypt(scell1.encrypt(self.data, self.context),
                           self.context))

        encrypted = scell1.encrypt(self.data, self.context)
        with self.assertRaises(ThemisError):
            scell2.decrypt(encrypted, self.incorrect_context)

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data, self.context)

        with self.assertRaises(ThemisError):
            scell3.decrypt(encrypted, self.context)

    def api_compatibility(self):
        # Make sure positional API uses keys, not passphrases
        scell_old = SCellSeal(self.key)
        scell_new = SCellSeal(key=self.key)

        encrypted = scell_old.encrypt(self.data)
        decrypted = scell_new.decrypt(encrypted)

        self.assertEqual(self.data, decrypted)


class SCellSealPassphraseTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellSeal(passphrase=None)
        with self.assertRaises(ThemisError):
            SCellSeal(passphrase=b'')
        with self.assertRaises(ThemisError):
            SCellSeal(passphrase=u'')

    def test_encoding(self):
        scell1 = SCellSeal(passphrase=u'passphrase'.encode('utf-16'))
        scell2 = SCellSeal(passphrase=u'passphrase', encoding='utf-16')

        data12 = scell1.decrypt(scell2.encrypt(self.data))
        data21 = scell2.decrypt(scell1.encrypt(self.data))

        self.assertEqual(self.data, data12)
        self.assertEqual(self.data, data21)

    def test_init_weird(self):
        # You can't use key and passphrase simultaneously
        with self.assertRaises(ThemisError):
            SCellSeal(key=self.key, passphrase=u'secrets')
        # You can't omit both of them too
        with self.assertRaises(ThemisError):
            SCellSeal()

    def test_encrypt_decrypt(self):
        scell1 = SCellSeal(passphrase=self.passphrase)
        scell2 = SCellSeal(passphrase=self.passphrase)
        scell3 = SCellSeal(passphrase=self.incorrect_passphrase)
        self.assertEqual(self.data, scell2.decrypt(scell1.encrypt(self.data)))
        with self.assertRaises(ThemisError):
            scell3.decrypt(scell1.encrypt(self.data))

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data)

    def test_encrypt_decrypt_context(self):
        scell1 = SCellSeal(passphrase=self.passphrase)
        scell2 = SCellSeal(passphrase=self.passphrase)
        scell3 = SCellSeal(passphrase=self.incorrect_passphrase)
        self.assertEqual(
            self.data,
            scell2.decrypt(scell1.encrypt(self.data, self.context),
                           self.context))

        encrypted = scell1.encrypt(self.data, self.context)
        with self.assertRaises(ThemisError):
            scell2.decrypt(encrypted, self.incorrect_context)

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data, self.context)

        with self.assertRaises(ThemisError):
            scell3.decrypt(encrypted, self.context)

    def api_compatibility(self):
        # Make sure positional API uses passphrases
        scell_old = SCellSealPassphrase(self.passphrase)
        scell_new = SCellSeal(passphrase=self.passphrase)

        encrypted = scell_old.encrypt(self.data)
        decrypted = scell_new.decrypt(encrypted)

        self.assertEqual(self.data, decrypted)


class SCellContextImprintTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellContextImprint(None)
        with self.assertRaises(ThemisError):
            SCellContextImprint(b'')
        with warnings.catch_warnings(record=True) as w:
            SCellContextImprint(u'passphrase')
            self.assertEqual(len(w), 1)
            self.assertTrue('master key should be "bytes"' in str(w[-1].message))

    def test_encrypt_decrypt(self):
        scell1 = SCellContextImprint(self.key)
        scell2 = SCellContextImprint(self.key)
        self.assertEqual(
            self.data,
            scell2.decrypt(scell1.encrypt(self.data, self.context),
                           self.context))

        encrypted = scell1.encrypt(self.data, self.context)
        decrypted = scell2.decrypt(encrypted, self.incorrect_context)
        self.assertNotEqual(self.data, decrypted)

        scell3 = SCellContextImprint(self.incorrect_key)
        self.assertNotEqual(self.data, scell3.decrypt(encrypted, self.context))

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data, self.context)

    def api_compatibility(self):
        # Make sure positional API uses keys, not passphrases
        scell_old = SCellContextImprint(self.key)
        scell_new = SCellContextImprint(key=self.key)

        encrypted = scell_old.encrypt(self.data)
        decrypted = scell_new.decrypt(encrypted)

        self.assertEqual(self.data, decrypted)


class SCellTokenProtectTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellTokenProtect(None)
        with self.assertRaises(ThemisError):
            SCellTokenProtect(b'')
        with warnings.catch_warnings(record=True) as w:
            SCellTokenProtect(u'passphrase')
            self.assertEqual(len(w), 1)
            self.assertTrue('master key should be "bytes"' in str(w[-1].message))

    def test_encrypt_decrypt(self):
        scell1 = SCellTokenProtect(self.key)
        scell2 = SCellTokenProtect(self.key)
        scell3 = SCellTokenProtect(self.incorrect_key)
        encrypted, token = scell1.encrypt(self.data)
        self.assertEqual(len(self.data), len(encrypted))
        self.assertEqual(self.data, scell2.decrypt(encrypted, token))

        with self.assertRaises(ThemisError):
            scell3.decrypt(encrypted, token)

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data)

    def test_encrypt_decrypt_context(self):
        scell1 = SCellTokenProtect(self.key)
        scell2 = SCellTokenProtect(self.key)
        scell3 = SCellTokenProtect(self.incorrect_key)
        encrypted, token = scell1.encrypt(self.data, self.context)
        self.assertEqual(len(self.data), len(encrypted))
        self.assertEqual(self.data,
                         scell2.decrypt(encrypted, token, self.context))

        encrypted, token = scell1.encrypt(self.data, self.context)
        with self.assertRaises(ThemisError):
            scell2.decrypt(encrypted, token, self.incorrect_context)

        with self.assertRaises(ThemisError):
            scell3.decrypt(encrypted, token, self.context)

        with self.assertRaises(ThemisError):
            scell1.encrypt(self.incorrect_data, self.context)

    def api_compatibility(self):
        # Make sure positional API uses keys, not passphrases
        scell_old = SCellTokenProtect(self.key)
        scell_new = SCellTokenProtect(key=self.key)

        encrypted, token = scell_old.encrypt(self.data)
        decrypted = scell_new.decrypt(encrypted, token)

        self.assertEqual(self.data, decrypted)


if __name__ == '__main__':
    unittest.main()
