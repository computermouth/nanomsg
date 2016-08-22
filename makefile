all: server client

server:
	gcc server.c -lnanomsg -o server

client:
	gcc client.c -lnanomsg -o client

clean:
	rm client server
