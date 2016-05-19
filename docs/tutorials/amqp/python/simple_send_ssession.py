import logging
import pika
from pythemis import smessage, ssession

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
# declare keys
client_pub = b"\x55\x45\x43\x32\x00\x00\x00\x2d\x13\x8b\xdf\x0c\x02\x1f\x09\x88\x39\xd9\x73\x3a\x84\x8f\xa8\x50\xd9\x2b\xed\x3d\x38\xcf\x1d\xd0\xce\xf4\xae\xdb\xcf\xaf\xcb\x6b\xa5\x4a\x08\x11\x21"
server_priv = b"\x52\x45\x43\x32\x00\x00\x00\x2d\x49\x87\x04\x6b\x00\xf2\x06\x07\x7d\xc7\x1c\x59\xa1\x8f\x39\xfc\x94\x81\x3f\x9e\xc5\xba\x70\x6f\x93\x08\x8d\xe3\x85\x82\x5b\xf8\x3f\xc6\x9f\x0b\xdf"


class SsessionTransport(ssession.mem_transport):
    def get_pub_key_by_id(self, user_id):
        if user_id != b"client":
            raise Exception("no such id")
        return client_pub


# connect to rabbitmq
connection = pika.BlockingConnection()
# create channel
channel = connection.channel()


#################
# SESSION HANDLING BLOCK
#################
channel.queue_declare(queue='ssession_response')
channel.queue_declare(queue='ssession_request')

session = ssession.ssession(b'server', server_priv, SsessionTransport())


# handler for responses from sender during establishing ssession
def on_request(ch, method, properties, body):
    decrypted_message = session.unwrap(body)
    if decrypted_message.is_control:
        logger.info('Come control message')
        channel.basic_publish(exchange='', routing_key='ssession_response',
                              body=decrypted_message)

    if session.is_established() == 1:
        logger.info('initializated ssession')
        ch.stop_consuming()

channel.basic_consume(on_request, 'ssession_request', no_ack=True)
# start establishing ssession
logger.info('start listening new ssession')
channel.start_consuming()
#################
# END SESSION HANDLING BLOCK
#################

# declare queue name
queue_name = 'ssession_queue'
# declare queue
result = channel.queue_declare(queue_name)

for i in range(10):
    message = 'message {}'.format(i)
    print('send "{}"'.format(message))
    # encrypt message
    encrypted_message = session.wrap(message.encode('utf-8'))
    # send to queue encrypted message
    channel.basic_publish(exchange='', routing_key=queue_name,
                          body=encrypted_message)
