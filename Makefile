# Makefile You should also prepare a makefile that generates the executable file deliver from
# deliver.c and the executable file server from server.c.
CC=gcc -O0  -pthread
all: server client
# compile the client program
client: client.o
# compile server program
server: server.o
clean:
	rm -f *.o client server