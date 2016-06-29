all: pipe crew server once mutex_attr cond_attr

pipe: pipe.c errors.h
	gcc -Wall -pedantic -std=c99 pipe.c -lpthread -o pipe.out

crew: crew.c errors.h
	gcc -Wall -pedantic -std=c99 crew.c  -D_XOPEN_SOURCE=700  -lpthread -o crew.out

server: server.c errors.h
	gcc -Wall -pedantic -std=c99 server.c  -D_XOPEN_SOURCE=700  -lpthread -o server.out

once: once.c errors.h
	gcc -Wall -pedantic -std=c99 once.c  -D_XOPEN_SOURCE=700  -lpthread -o once.out

mutex_attr: mutex_attr.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_attr.c  -lpthread -o mutex_attr.out

cond_attr: cond_attr.c errors.h
	gcc -Wall -pedantic -std=c99 cond_attr.c  -lpthread -o cond_attr.out