all: server client

server: server.c
	gcc -Wall server.c -o out/server

client: client.c
	gcc -Wall client.c -o out/client

run:
	./out/server 127.0.0.1 &
	./out/client 127.0.0.1

run_client:
	./out/client 127.0.0.1

run_server:
	./out/server 127.0.0.1 &

clean: 
	rm -f out/*