CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: client.c
	${CC} client.c -o client

server: server.c linkedList.o
	${CC} server.c -o server linkedList.o

linkedList.o: linkedList.c
	${CC} -c linkedList.c

clean:
	rm -f *.o *~
