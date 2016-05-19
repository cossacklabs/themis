import pika
from pythemis import smessage


# declare keys
client_pub = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21"
server_priv = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf"

# create smessage object
smessage = smessage.smessage(server_priv, client_pub)

# connect to rabbitmq
connection = pika.BlockingConnection()
# create channel
channel = connection.channel()
# declare queue
channel.queue_declare(queue='smessage_queue')


def on_request(ch, method, props, body):
    # decrypt message
    message = smessage.unwrap(body)  # decrypt received message
    print('recieved message: "{}"'.format(message.decode('utf-8')))
    # send acknowledge to rabbitmq
    ch.basic_ack(delivery_tag=method.delivery_tag)

# listen queue and register callback
channel.basic_consume(on_request, queue='smessage_queue')
print('start consuming')
channel.start_consuming()
