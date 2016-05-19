import pika
from pythemis.smessage import smessage

server_priv = b'REC2\x00\x00\x00-,w\xbf\xf2\x00\xb4\xd52\x9eg\xbcV\xaaU\x0e\xa8\xc6;\xea\xc1\xb38vo\x07\xaf\x9d\x0e\xce\xc7[AD3(v\xe6'

# declare keys of all clients
client_pub = [b'UEC2\x00\x00\x00-}3\\N\x03\xe22\x1aV")\xab\\\xb2\xef\x88H\x97^\xf9r\xa3\xa5\xabIq\xb99\x05\x16\xbb\x187\x97\xf41!',
              b'UEC2\x00\x00\x00-4H\xcaf\x02\xf6\x88p;\x18 2}\xf4}\xca\xd1M%\xab\xcd\xbf\x7f\x07*\x99\xd6/\xae\x1f\xbd\xd8\x06\x8eE0\xa3']

connection = pika.BlockingConnection()
channel = connection.channel()

channel.exchange_declare(exchange='logs', type='fanout')

message = "info: Hello World!"
# send using all public keys
for key in client_pub:
    smessage_instance = smessage(server_priv, key)
    encrypted_message = smessage_instance.wrap(message.encode('utf-8'))
    channel.basic_publish(exchange='logs', routing_key='',
                          body=encrypted_message)
    print(" [x] Sent %r" % message)
connection.close()
