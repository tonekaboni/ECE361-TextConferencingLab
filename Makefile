# Makefile You should also prepare a makefile that generates the executable file deliver from
# deliver.c and the executable file server from server.c.
CC=gcc -g
all: server deliver
# compile the client program
deliver: deliver.o
# compile server program
server: server.o
clean:
	rm -f *.o deliver server