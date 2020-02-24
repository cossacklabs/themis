import base64

from pythemis.scell import SCellTokenProtect
from pythemis.scell import SCellSeal
from pythemis.scell import SCellContextImprint

message = "i'm plain text message"
context = b"somecontext"
master_key = base64.b64decode("bm8sIHRoaXMgaXMgbm90IGEgdmFsaWQgbWFzdGVyIGtleQ==")
passphrase = b"secret passphrase"


print("# Secure Cell in Seal mode\n")

print("## Master key API\n")

scellMK = SCellSeal(key=master_key)

encrypted_message = scellMK.encrypt(message.encode('utf-8'))
print("Encrypted: " + base64.b64encode(encrypted_message).decode('ascii'))

decrypted_message = scellMK.decrypt(encrypted_message).decode('utf-8')
print("Decrypted: " + decrypted_message)

# Visit https://docs.cossacklabs.com/simulator/data-cell/
print("")
encrypted_message_string = "AAEBQAwAAAAQAAAAEQAAAC0fCd2mOIxlDUORXz8+qCKuHCXcDii4bMF8OjOCOqsKEdV4+Ga2xTHPMupFvg=="
decrypted_message_from_string = base64.b64decode(encrypted_message_string)
decrypted_message = scellMK.decrypt(decrypted_message_from_string).decode('utf-8')
print("Decrypted (simulator): " + decrypted_message)

print("")


print("## Passphrase API\n")

scellPW = SCellSeal(passphrase=passphrase)

encrypted_message = scellPW.encrypt(message.encode('utf-8'))
print("Encrypted: " + base64.b64encode(encrypted_message).decode('ascii'))

decrypted_message = scellPW.decrypt(encrypted_message).decode('utf-8')
print("Decrypted: " + decrypted_message)

print("")


print("# Secure Cell in Token Protect mode\n")

scellTP = SCellTokenProtect(key=master_key)

encrypted_message, auth_token = scellTP.encrypt(message.encode('utf-8'))
print("Encrypted:  " + base64.b64encode(encrypted_message).decode('ascii'))
print("Auth token: " + base64.b64encode(auth_token).decode('ascii'))

decrypted_message = scellTP.decrypt(encrypted_message, auth_token).decode('utf-8')
print("Decrypted:  " + decrypted_message)

print("")


print("# Secure Cell in Context Imprint mode\n")

scellCI = SCellContextImprint(key=master_key)

encrypted_message = scellCI.encrypt(message.encode('utf-8'), context)
print("Encrypted: " + base64.b64encode(encrypted_message).decode('ascii'))

decrypted_message = scellCI.decrypt(encrypted_message, context).decode('utf-8')
print("Decrypted: " + decrypted_message)

print("")
