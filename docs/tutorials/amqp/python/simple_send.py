import pika

# declare queue name
queue_name = 'smessage_queue'

# connect to rabbitmq
connection = pika.BlockingConnection()
# create channel
channel = connection.channel()
# declare queue
result = channel.queue_declare(queue_name)

for i in range(10):
    message = 'message {}'.format(i)
    print('send "{}"'.format(message))
    # send to queue encrypted message
    channel.basic_publish(exchange='', routing_key=queue_name,
                          body=message)
