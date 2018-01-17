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

from pythemis.scell import SCellTokenProtect, SCellSeal, SCellContextImprint
from pythemis.exception import ThemisError


class BaseSCellTestMixin(unittest.TestCase):
    def setUp(self):
        self.context = b'some context'
        self.incorrect_context = b'another context'
        self.key = b'some key'
        self.incorrect_key = b'another key'
        self.data = b'some data'
        # zero length
        self.incorrect_data = b''
        super(BaseSCellTestMixin, self).setUp()


class SCellSealTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellSeal(None)

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
            SCellSeal(self.incorrect_key).decrypt(encrypted, self.context)


class SCellContextImprintTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellContextImprint(None)

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


class SCellTokenProtectTest(BaseSCellTestMixin):
    def test_init(self):
        with self.assertRaises(ThemisError):
            SCellTokenProtect(None)

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


if __name__ == '__main__':
    unittest.main()
