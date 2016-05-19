import logging
import pika

from pythemis import ssession

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# declare keys
client_priv = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x51\xf4\xaa\x72\x00\x9f\x0f\x09\xce\xbe\x09\x33\xc2\x5e\x9a\x05\x99\x53\x9d\xb2\x32\xa2\x34\x64\x7a\xde\xde\x83\x8f\x65\xa9\x2a\x14\x6d\xaa\x90\x01"
server_pub = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x75\x58\x33\xd4\x02\x12\xdf\x1f\xe9\xea\x48\x11\xe1\xf9\x71\x8e\x24\x11\xcb\xfd\xc0\xa3\x6e\xd6\xac\x88\xb6\x44\xc2\x9a\x24\x84\xee\x50\x4c\x3e\xa0"

class SsessionTransport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):
        if user_id != b"server":
            raise Exception("no such id")
        return server_pub


# connect to rabbitmq
connection = pika.BlockingConnection()
# create channel
channel = connection.channel()

#################
# SESSION HANDLING BLOCK
#################
channel.queue_declare(queue='ssession_response')
channel.queue_declare(queue='ssession_request')

session = ssession.ssession(b'client', client_priv, SsessionTransport())


# handler for responses from sender during establishing ssession
def on_response(ch, method, properties, body):
    decrypted_message = session.unwrap(body)
    if decrypted_message.is_control:
        logger.info('Come control message')
        channel.basic_publish(exchange='', routing_key='ssession_request',
                              body=decrypted_message)

    if session.is_established() == 1:
        logger.info('initializated ssession')
        ch.stop_consuming()

init_message = session.connect_request()
channel.basic_consume(on_response, 'ssession_response', no_ack=True)
# start establishing ssession
logger.info('start establishing ssession')
channel.basic_publish(exchange='', routing_key='ssession_request',
                      body=init_message)
channel.start_consuming()

#################
# END SESSION HANDLING BLOCK
#################


def on_request(ch, method, props, body):
    message = session.unwrap(body)
    # decrypt message
    logger.info('recieved message: "{}"'.format(message.decode('utf-8')))
    # send acknowledge to rabbitmq
    ch.basic_ack(delivery_tag=method.delivery_tag)

# declare queue
channel.queue_declare(queue='ssession_queue')
# listen queue and register callback
channel.basic_consume(on_request, queue='ssession_queue')
logger.info('start consuming')
channel.start_consuming()
