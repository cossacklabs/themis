import pika

# connect to rabbitmq
connection = pika.BlockingConnection()
# create channel
channel = connection.channel()
# declare queue
channel.queue_declare(queue='smessage_queue')


def on_request(ch, method, props, body):
    # decrypt message
    print('recieved message: "{}"'.format(body.decode('utf-8')))
    # send acknowledge to rabbitmq
    ch.basic_ack(delivery_tag=method.delivery_tag)

# listen queue and register callback
channel.basic_consume(on_request, queue='smessage_queue')
print('start consuming')
channel.start_consuming()
