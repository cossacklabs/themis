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
from pythemis import scell
from pythemis.exception import themis_exception

class TestSCell(unittest.TestCase):
    def setUp(self):
        self.key=b"This is test key"
        self.message=b"This is test message"
        self.context=b"This is test context"

    def testSeal(self):
        with self.assertRaises(themis_exception):
            enc=scell.scell_seal("")
        with self.assertRaises(TypeError):
            enc=scell.scell_seal(None)
        with self.assertRaises(TypeError):
            enc=scell.scell_seal(112233)
        enc=scell.scell_seal(self.key)
        with self.assertRaises(themis_exception):
            encrypted_message=enc.encrypt("")
        with self.assertRaises(TypeError):
            encrypted_message=enc.encrypt(None)
        encrypted_message=enc.encrypt(self.message)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(b"".join([encrypted_message,b"11"]))
        decrypted_message=enc.decrypt(encrypted_message)
        self.assertEqual(self.message, decrypted_message)

    def testSealWithContext(self):
        with self.assertRaises(themis_exception):
            enc=scell.scell_seal("")
        with self.assertRaises(TypeError):
            enc=scell.scell_seal(None)
        with self.assertRaises(TypeError):
            enc=scell.scell_seal(112233)
        enc=scell.scell_seal(self.key)
        with self.assertRaises(themis_exception):
            encrypted_message=enc.encrypt("", self.context)
        with self.assertRaises(TypeError):
            encrypted_message=enc.encrypt(None, self.context)
        encrypted_message=enc.encrypt(self.message, self.context)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(b"".join([encrypted_message,b"11"]), self.context)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(encrypted_message)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(encrypted_message, None)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(encrypted_message, b"".join([self.context,b"11"]))
        decrypted_message=enc.decrypt(encrypted_message, self.context)
        self.assertEqual(self.message, decrypted_message)

    def testTokenProtect(self):
        with self.assertRaises(themis_exception):
            enc=scell.scell_token_protect("")
        with self.assertRaises(TypeError):
            enc=scell.scell_token_protect(None)
        with self.assertRaises(TypeError):
            enc=scell.scell_token_protect(112233)
        enc=scell.scell_token_protect(self.key)
        with self.assertRaises(themis_exception):
            encrypted_message, token=enc.encrypt("")
        with self.assertRaises(TypeError):
            encrypted_message, token=enc.encrypt(None)
        encrypted_message, token=enc.encrypt(self.message)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(b"".join([encrypted_message,b"11"]), token)
        with self.assertRaises(TypeError):
            decrypted_message=enc.decrypt(encrypted_message, None)
        decrypted_message=enc.decrypt(encrypted_message, token)
        self.assertEqual(self.message, decrypted_message)

    def testTokenProtectWithContext(self):
        with self.assertRaises(themis_exception):
            enc=scell.scell_token_protect("")
        with self.assertRaises(TypeError):
            enc=scell.scell_token_protect(None)
        with self.assertRaises(TypeError):
            enc=scell.scell_token_protect(112233)
        enc=scell.scell_token_protect(self.key)
        with self.assertRaises(themis_exception):
            encrypted_message, token=enc.encrypt("", self.context)
        with self.assertRaises(TypeError):
            encrypted_message, token=enc.encrypt(None, self.context)
        encrypted_message, token=enc.encrypt(self.message, self.context)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(b"".join([encrypted_message,b"11"]), token, self.context)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(encrypted_message, token)
        with self.assertRaises(TypeError):
            decrypted_message=enc.decrypt(encrypted_message, None, self.context)
        with self.assertRaises(themis_exception):
            decrypted_message=enc.decrypt(encrypted_message, token, b"".join([self.context,b"11"]))
        decrypted_message=enc.decrypt(encrypted_message, token, self.context)
        self.assertEqual(self.message, decrypted_message)

    def testContextImprint(self):
        with self.assertRaises(themis_exception):
            enc=scell.scell_context_imprint("")
        with self.assertRaises(TypeError):
            enc=scell.scell_context_imprint(None)
        with self.assertRaises(TypeError):
            enc=scell.scell_context_imprint(112233)
        enc=scell.scell_context_imprint(self.key)
        with self.assertRaises(TypeError):
            encrypted_message=enc.encrypt(self.message)
        with self.assertRaises(TypeError):
            encrypted_message=enc.encrypt(None, self.context)
        encrypted_message=enc.encrypt(self.message, self.context)
        decrypted_message=enc.decrypt(b"".join([encrypted_message,b"11"]), self.context)
        self.assertNotEqual(self.message, decrypted_message)
        decrypted_message=enc.decrypt(encrypted_message, b"".join([self.context,b"11"]))
        self.assertNotEqual(self.message, decrypted_message)
        with self.assertRaises(TypeError):
            decrypted_message=enc.decrypt(encrypted_message)
        decrypted_message=enc.decrypt(encrypted_message, self.context)
        self.assertEqual(self.message, decrypted_message)

if __name__ == '__main__':
    unittest.main()