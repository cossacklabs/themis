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

#encrypt all leafs data in xml tree.
#as format preserving the path to leaf is additional cotext for enryption parameters deriving

import xml.etree.ElementTree as ET;
import base64;
from themis import scell;

password="password";


def enc_children(node, context):
    if len(node)!=0:
        for i in range(0, len(node)):
            enc_children(node[i], context+"/"+node.tag);
    else:
        if node.text!=None:
            node.text=base64.b64encode(enc.encrypt(node.text, context+"/"+node.tag)); #encrypt leaf data and replace it in file by base64 encoding

def dec_children(node, context):
    if len(node)!=0:
        for i in range(0, len(node)):
            dec_children(node[i], context+"/"+node.tag);
    else:
        if node.text!=None:
            node.text=enc.decrypt(base64.b64decode(node.text), context+"/"+node.tag); #decrypt base64 encoded leaf data and replace it by plain value

#encoding file data.xml and save result to encoded_data.xml
tree = ET.parse('data.xml');
root = tree.getroot();
enc=scell.scell_full(password);
enc_children(root, "");
tree.write("encoded_data.xml");

#decoding file encoded_data.xml and save result to decoded_data.xml
tree2 = ET.parse('encoded_data.xml');
root2 = tree2.getroot();
dec_children(root2, "");
tree2.write("decoded_data.xml");

