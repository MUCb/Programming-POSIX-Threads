all: mutex_attr cond_attr thread_attr cancel cancel_disable cancel_async cancel_cleanup

mutex_attr: mutex_attr.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_attr.c  -lpthread -o mutex_attr.out

cond_attr: cond_attr.c errors.h
	gcc -Wall -pedantic -std=c99 cond_attr.c  -lpthread -o cond_attr.out

thread_attr: thread_attr.c errors.h
	gcc -Wall -pedantic -std=c99 thread_attr.c  -lpthread -o thread_attr.out

cancel: cancel.c errors.h
	gcc -Wall -pedantic -std=c99 cancel.c  -lpthread -DDEBUG -o cancel.out

cancel_disable: cancel_disable.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_disable.c  -lpthread -o cancel_disable.out

cancel_async: cancel_async.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_async.c  -lpthread -o cancel_async.out

cancel_cleanup: cancel_cleanup.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_cleanup.c  -lpthread -o cancel_cleanup.out