#!/usr/bin/env python
import time
import logging
from threading import Thread

import pika

from pythemis import ssession

logger = logging.getLogger(__name__)
logging.basicConfig()
logging.getLogger(__name__)
server_priv = b'REC2\x00\x00\x00-,w\xbf\xf2\x00\xb4\xd52\x9eg\xbcV\xaaU\x0e\xa8\xc6;\xea\xc1\xb38vo\x07\xaf\x9d\x0e\xce\xc7[AD3(v\xe6'
# declare keys for all clients
client_pub = {
    b'user_0': b'UEC2\x00\x00\x00-}3\\N\x03\xe22\x1aV")\xab\\\xb2\xef\x88H\x97^\xf9r\xa3\xa5\xabIq\xb99\x05\x16\xbb\x187\x97\xf41!',
    b'user_1': b'UEC2\x00\x00\x00-4H\xcaf\x02\xf6\x88p;\x18 2}\xf4}\xca\xd1M%\xab\xcd\xbf\x7f\x07*\x99\xd6/\xae\x1f\xbd\xd8\x06\x8eE0\xa3'
}


class SSessionTransport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):
        return client_pub[user_id]


connection = pika.BlockingConnection()
channel = connection.channel()

#################
# SESSION HANDLING BLOCK
#################
# create exchange for new ssessions
channel.exchange_declare(exchange='ssession', type='topic')
# create queue for new ssessions
request_result = channel.queue_declare(exclusive=True)
request_queue_name = request_result.method.queue
# listen all requests for creating session
request_routing_key = '*.request'
# link queue to exchange
channel.queue_bind(exchange='ssession', queue=request_queue_name,
                   routing_key=request_routing_key)

# storage for ssessions with recieivers
sessions = {}


def handle_sessions():
    def on_request(ch, method, properties, body):
        client_id = method.routing_key.split('.')[0].encode('ascii')
        if client_id in sessions:
            logger.info('come ssession request from client: {}'.format(
                client_id.decode('ascii')))
            session = sessions[client_id]
        else:
            logger.info('come new ssession request from client: {}'.format(
                client_id.decode('ascii')))
            session = ssession.ssession(b'server', server_priv,
                                        SSessionTransport())
            sessions[client_id] = session
        msg = session.unwrap(body)
        if msg.is_control:
            logger.info('ssession request is control')
            response_routing_key = '{}.response'.format(
                client_id.decode('ascii'))
            channel.basic_publish(exchange='ssession',
                                  routing_key=response_routing_key, body=msg)

    channel.basic_consume(on_request, queue=request_queue_name)
    channel.start_consuming()

# create thread that will establish ssession with recievers
logger.info('start thread for ssessions')
handle_sessions_thread = Thread(target=handle_sessions)
handle_sessions_thread.start()

#################
# END SESSION HANDLING BLOCK
#################

# create exchange for data messages
channel.exchange_declare(exchange='ssession_data', type='fanout')

num = 0
while True:
    message = 'message {}'.format(num)
    for client in sessions:
        if sessions[client].is_established() == 1:
            encrypted_message = sessions[client].wrap(
                message.encode('utf-8'))
            message = 'send message "{}" to client {}'.format(
                message, client.decode('ascii')
            )
            logger.info(message)
            print(message)
            channel.basic_publish(exchange='ssession_data', routing_key='',
                                  body=encrypted_message)
    num += 1
    time.sleep(1)
