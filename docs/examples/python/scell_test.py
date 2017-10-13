import base64

from pythemis.scell import SCellSeal

message = 'message to encrypt'
passwrd = b"pass"

scell = SCellSeal(passwrd)

print("encrypting...")
encrypted_message = scell.encrypt(message.encode('utf-8'))
encrypted_message_string = base64.b64encode(encrypted_message)
print(encrypted_message_string)

print("decrypting from binary...")
decrypted_message = scell.decrypt(encrypted_message).decode('utf-8')
print(decrypted_message)


print("decrypting from string...")
# check https://themis.cossacklabs.com/data-simulator/cell/
encrypted_message_string = "AAEBQAwAAAAQAAAADQAAACoEM9MbJzEu2RDuRoGzcQgN4jchys0q+LLcsbfUDV3M2eg/FhygH1ns"
decrypted_message_from_string = base64.b64decode(encrypted_message_string)
decrypted_message = scell.decrypt(decrypted_message_from_string).decode('utf-8')
print(decrypted_message)