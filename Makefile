Publisher: 
	gcc publisher.c -o $@.out -lpaho-mqtt3c

Subscriber:
	gcc subscriber.c -o $@.out -lpaho-mqtt3c

run-pub: Publisher
	./Publisher.out

run-sub: Subscriber
	./Subscriber.out

run-pub-sub:
	gcc pubsub.c -o pubsub.out -lpaho-mqtt3c -lpthread

clean:
	rm -rf Publisher.out Subscriber.out