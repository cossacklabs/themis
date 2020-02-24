#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License")
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
"""
encrypt all leafs data in xml tree.
as format preserving the path to leaf is additional cotext for enryption parameters deriving
"""

import xml.etree.ElementTree as ET
import base64
from pythemis import scell

master_key = base64.b64decode(b'c2NlbGxfeG1sX2Zvcm1hdC1wcmVzZXJ2aW5nX2VuY3J5cHRpb24ucHk=')


def encrypt_children(node, context):
    if len(node):
        for i in range(0, len(node)):
            encrypt_children(node[i], context + "/" + node.tag)
    else:
        if node.text:
            # encrypt leaf data and replace it in file by base64 encoding
            node.text = base64.b64encode(
                encryptor.encrypt(node.text.encode('utf-8'),
                                  context + "/" + node.tag)).decode('ascii')


def decrypt_children(node, context):
    if len(node):
        for i in range(0, len(node)):
            decrypt_children(node[i], context + "/" + node.tag)
    else:
        if node.text:
            # decrypt base64 encoded leaf data and replace it by plain value
            node.text = encryptor.decrypt(
                base64.b64decode(node.text),
                context + "/" + node.tag).decode('utf-8')


# encoding file 'example_data/test.xml' and save result to encoded_data.xml
tree = ET.parse('example_data/test.xml')
root = tree.getroot()
encryptor = scell.SCellSeal(master_key)
encrypt_children(root, "")
tree.write("encoded_data.xml")

# decoding file encoded_data.xml and save result to decoded_data.xml
tree2 = ET.parse('encoded_data.xml')
root2 = tree2.getroot()
decrypt_children(root2, "")
tree2.write("decoded_data.xml")
