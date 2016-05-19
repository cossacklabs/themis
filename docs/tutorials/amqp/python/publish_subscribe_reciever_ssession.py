#!/usr/bin/env python
import pika
import sys
import logging
from pythemis import ssession
from pythemis.exception import themis_exception

logger = logging.getLogger(__name__)
logging.basicConfig()
logging.getLogger(__name__)
server_pub = b'UEC2\x00\x00\x00-g\xa2\xfa\xcd\x028#\x96/<]>\xbcr\x9bO\xeb(\x10=\x07\x1a\xe3\xaa\x10\xb2r\x87\xb0\xa4\xdb\x86\xf3\xcd\xf8n\x9b'
client_priv = [
    b'REC2\x00\x00\x00-f\x0e\xc9e\x00J\xbe}V\xd7KPj\xed\xd9R\x17\xd4\xe0\xe3\xa4\x8fT\nUu=kZ\x1d\x04\xbb`\xc2\xd43\xde',
    b'REC2\x00\x00\x00-r\xc4\xc8\xd8\x00\xab\x91\xf1l\x0cF2Q\x91l\xfb\xcc=z(\xdf\xcc\x03\xa5.\x99\x05k\xadR*\xb7d\x97\xbb\xac='
]


class SSessionTransport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):
        if user_id != b"server":
            raise Exception("no such id")
        return server_pub


connection = pika.BlockingConnection()
channel = connection.channel()


#################
# SESSION ESTABLISHING BLOCK
#################
# create exchange for establishing ssessions
channel.exchange_declare(exchange='ssession', type='topic')
# create queue for responses from sender on request for establishing ssessions
response_result = channel.queue_declare(exclusive=True)
response_queue_name = response_result.method.queue

user_num = int(input("Client number: ")) % 2
user_id = 'user_{}'.format(user_num)
# routing key for current user for requests
request_routing_key = '{}.request'.format(user_id)
# routing key for current user for response
response_routing_key = '{}.response'.format(user_id)
# store responses in response queue
channel.queue_bind(exchange='ssession', queue=response_queue_name,
                   routing_key=response_routing_key)

# create ssession that will be used in communicating with sender
session = ssession.ssession(user_id.encode('ascii'), client_priv[user_num],
                            SSessionTransport())


# handler for responses from sender during establishing ssession
def on_response(ch, method, properties, body):
    decrypted_message = session.unwrap(body)
    if decrypted_message.is_control:
        logger.info('Came control message')
        channel.basic_publish(exchange='ssession',
                              routing_key=request_routing_key,
                              body=decrypted_message)
        return
    logger.info('initializated ssession')
    ch.stop_consuming()

init_message = session.connect_request()
channel.basic_consume(on_response, response_queue_name, no_ack=True)
logger.info('start establishing ssession')
channel.basic_publish(exchange='ssession',
                      routing_key=request_routing_key,
                      body=init_message)
channel.start_consuming()
#################
# END SESSION ESTABLISHING BLOCK
#################

# here session is established

# handler for recieved data messages
def on_request(ch, method, props, body):
    # decrypt message
    try:
        message = session.unwrap(body)  # decrypt received message
    except themis_exception:
        logger.info('Recieved message for another reciever')
        ch.basic_ack(delivery_tag=method.delivery_tag)
        return
    msg = 'Recieved message: {}'.format(message.decode('utf-8'))
    logger.info(msg)
    print(msg)
    # send acknowledge to rabbitmq
    ch.basic_ack(delivery_tag=method.delivery_tag)

# create exchange for data messages
channel.exchange_declare(exchange='ssession_data', type='fanout')
# create queue where will be stored data messages
data_result = channel.queue_declare(exclusive=True)
queue_name = data_result.method.queue
# bind queue to exchange
channel.queue_bind(exchange='ssession_data', queue=queue_name)

# listen queue and register callback
channel.basic_consume(on_request, queue=queue_name)
logger.info('start listening data messages')
channel.start_consuming()
