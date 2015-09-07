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
from pythemis import scomparator
from pythemis.exception import themis_exception

from collections import deque
from time import sleep
from threading import Thread

class TestSComparator(unittest.TestCase):
    def setUp(self):
        self.message=b"This is test message"
        self.message1=b"This is test message2"

    def testComparation(self):
        alice=scomparator.scomparator(self.message)
        bob=scomparator.scomparator(self.message)
        data=alice.begin_compare()
        while alice.result() == scomparator.SCOMPARATOR_CODES.NOT_READY and bob.result() == scomparator.SCOMPARATOR_CODES.NOT_READY:
            with self.assertRaises(themis_exception):
                data=alice.proceed_compare(bob.proceed_compare(b"aa"+data))
            data=alice.proceed_compare(bob.proceed_compare(data))
        self.assertNotEqual(alice.result(), scomparator.SCOMPARATOR_CODES.NOT_MATCH)
        self.assertNotEqual(bob.result(), scomparator.SCOMPARATOR_CODES.NOT_MATCH)

    def testComparation2(self):
        alice=scomparator.scomparator(self.message)
        bob=scomparator.scomparator(self.message1)
        data=alice.begin_compare()
        while alice.result() == scomparator.SCOMPARATOR_CODES.NOT_READY and bob.result() == scomparator.SCOMPARATOR_CODES.NOT_READY:
            data=alice.proceed_compare(bob.proceed_compare(data))
        self.assertEqual(alice.result(), scomparator.SCOMPARATOR_CODES.NOT_MATCH)
        self.assertEqual(bob.result(), scomparator.SCOMPARATOR_CODES.NOT_MATCH)

if __name__ == '__main__':
    unittest.main()