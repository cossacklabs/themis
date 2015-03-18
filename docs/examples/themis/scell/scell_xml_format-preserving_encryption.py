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
            node.text=base64.b64encode(enc.encrypt(node.text, context+"/"+node.tag));

def dec_children(node, context):
    if len(node)!=0:
        for i in range(0, len(node)):
            dec_children(node[i], context+"/"+node.tag);
    else:
        if node.text!=None:
            node.text=enc.decrypt(base64.b64decode(node.text), context+"/"+node.tag);

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

