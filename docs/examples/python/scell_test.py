import base64

from pythemis.scell import SCellTokenProtect
from pythemis.scell import SCellSeal
from pythemis.scell import SCellContextImprint

message = "i'm plain text message"
passwrd = b"pass"
context = b"somecontext"


print("running secure cell in seal mode...")

scell = SCellSeal(passwrd)

print("encrypting...")
encrypted_message = scell.encrypt(message.encode('utf-8'))
encrypted_message_string = base64.b64encode(encrypted_message)
print(encrypted_message_string)

print("decrypting from binary... --> ")
decrypted_message = scell.decrypt(encrypted_message).decode('utf-8')
print(decrypted_message)


print("decrypting from string... --> ")
# check https://themis.cossacklabs.com/data-simulator/cell/
encrypted_message_string = "AAEBQAwAAAAQAAAADQAAACoEM9MbJzEu2RDuRoGzcQgN4jchys0q+LLcsbfUDV3M2eg/FhygH1ns"
decrypted_message_from_string = base64.b64decode(encrypted_message_string)
decrypted_message = scell.decrypt(decrypted_message_from_string).decode('utf-8')
print(decrypted_message)


print("----------------------")
print("running secure cell in token protect mode...")

scellTP = SCellTokenProtect(passwrd)

print("encrypting...")
encrypted_message, additional_auth_data = scellTP.encrypt(message.encode('utf-8'))
encrypted_message_string = base64.b64encode(encrypted_message)
print(encrypted_message_string)

print("decrypting from binary... --> ")
decrypted_message = scellTP.decrypt(encrypted_message, additional_auth_data).decode('utf-8')
print(decrypted_message)


print("----------------------")
print("running secure cell in context imprint mode...")


scellCI = SCellContextImprint(passwrd)

print("encrypting...")
encrypted_message = scellCI.encrypt(message.encode('utf-8'), context)
encrypted_message_string = base64.b64encode(encrypted_message)
print(encrypted_message_string)

print("decrypting from binary... --> ")
decrypted_message = scellCI.decrypt(encrypted_message, context).decode('utf-8')
print(decrypted_message)