#key generation example
import sys
from themis import skeygen

alg="EC";

obj = skeygen.themis_gen_key_pair(alg);
private_key = obj.export_private_key();
public_key = obj.export_public_key();

private_key_file = open(sys.argv[1],"w");
private_key_file.write(private_key);
private_key_file.close();
public_key_file = open(sys.argv[2],"w");
public_key_file.write(public_key);
public_key_file.close();
