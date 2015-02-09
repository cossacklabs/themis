import sys
from skeygen import *

alg="EC";

obj = themis_gen_key_pair(alg);
private_key = obj.export_private_key();
public_key = obj.export_public_key();

private_key_file = open(sys.argv[1],"w");
private_key_file.write(private_key[0][:private_key[1]]);
private_key_file.close();
public_key_file = open(sys.argv[2],"w");
public_key_file.write(public_key[0][:public_key[1]]);
public_key_file.close();
