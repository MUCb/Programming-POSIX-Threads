all: mutex_attr cond_attr thread_attr

mutex_attr: mutex_attr.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_attr.c  -lpthread -o mutex_attr.out

cond_attr: cond_attr.c errors.h
	gcc -Wall -pedantic -std=c99 cond_attr.c  -lpthread -o cond_attr.out

thread_attr: thread_attr.c errors.h
	gcc -Wall -pedantic -std=c99 thread_attr.c  -lpthread -o thread_attr.out