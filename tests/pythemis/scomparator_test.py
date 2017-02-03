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


class SComparatorTest(unittest.TestCase):
    def setUp(self):
        self.message = b"This is test message"
        self.message1 = b"This is test message2"

    def testComparation(self):
        alice = scomparator.SComparator(self.message)
        bob = scomparator.SComparator(self.message)
        data = alice.begin_compare()
        while not (alice.is_compared() and bob.is_compared()):
            data = alice.proceed_compare(bob.proceed_compare(data))
        self.assertTrue(alice.is_equal())
        self.assertTrue(bob.is_equal())

    def testComparation2(self):
        alice = scomparator.SComparator(self.message)
        bob = scomparator.SComparator(self.message1)
        data = alice.begin_compare()
        while not (alice.is_compared() and bob.is_compared()):
            data = alice.proceed_compare(bob.proceed_compare(data))
        self.assertFalse(alice.is_equal())
        self.assertFalse(bob.is_equal())


if __name__ == '__main__':
    unittest.main()
