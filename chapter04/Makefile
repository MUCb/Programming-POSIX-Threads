all: pipe crew server

pipe: pipe.c errors.h
	gcc -Wall -pedantic -std=c99 pipe.c -lpthread -o pipe.out

crew: crew.c errors.h
	gcc -Wall -pedantic -std=c99 crew.c  -D_XOPEN_SOURCE=700  -lpthread -o crew.out

server: server.c errors.h
	gcc -Wall -pedantic -std=c99 server.c  -D_XOPEN_SOURCE=700  -lpthread -o server.out
