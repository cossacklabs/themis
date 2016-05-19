#!/usr/bin/env python
import pika
import sys

from pythemis.exception import themis_exception
from pythemis.smessage import smessage

client_number = int(input("Client number: ")) % 2
server_pub = b'UEC2\x00\x00\x00-g\xa2\xfa\xcd\x028#\x96/<]>\xbcr\x9bO\xeb(\x10=\x07\x1a\xe3\xaa\x10\xb2r\x87\xb0\xa4\xdb\x86\xf3\xcd\xf8n\x9b'
client_priv = [b'REC2\x00\x00\x00-f\x0e\xc9e\x00J\xbe}V\xd7KPj\xed\xd9R\x17\xd4\xe0\xe3\xa4\x8fT\nUu=kZ\x1d\x04\xbb`\xc2\xd43\xde',
               b'REC2\x00\x00\x00-r\xc4\xc8\xd8\x00\xab\x91\xf1l\x0cF2Q\x91l\xfb\xcc=z(\xdf\xcc\x03\xa5.\x99\x05k\xadR*\xb7d\x97\xbb\xac=']


connection = pika.BlockingConnection()
channel = connection.channel()

channel.exchange_declare(exchange='logs', type='fanout')

result = channel.queue_declare(exclusive=True)
queue_name = result.method.queue

channel.queue_bind(exchange='logs', queue=queue_name)

print(' [*] Waiting for logs. To exit press CTRL+C')


def callback(ch, method, properties, body):
    smessage_instance = smessage(client_priv[client_number], server_pub)
    try:
        message = smessage_instance.unwrap(body)
        print("Client {}: Received {}".format(client_number,
                                              message.decode('utf-8')))
    except themis_exception:
        print('message to another user')

channel.basic_consume(callback, queue=queue_name, no_ack=True)
channel.start_consuming()
