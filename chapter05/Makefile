all: mutex_attr cond_attr thread_attr cancel cancel_disable \
	cancel_async cancel_cleanup cancel_subcontract tsd_once tsd_destructor \
	sched_attr sched_thread

mutex_attr: mutex_attr.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_attr.c  -lpthread -o mutex_attr.out

cond_attr: cond_attr.c errors.h
	gcc -Wall -pedantic -std=c99 cond_attr.c  -lpthread -o cond_attr.out

thread_attr: thread_attr.c errors.h
	gcc -Wall -pedantic -std=c99 thread_attr.c  -lpthread -o thread_attr.out

cancel: cancel.c errors.h
	gcc -Wall -pedantic -std=c99 cancel.c  -lpthread -DDEBUG -o cancel.out

cancel_disable: cancel_disable.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_disable.c  \
                                        -lpthread -o cancel_disable.out

cancel_async: cancel_async.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_async.c  -lpthread -o cancel_async.out

cancel_cleanup: cancel_cleanup.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_cleanup.c  \
                                        -lpthread -o cancel_cleanup.out

cancel_subcontract: cancel_subcontract.c errors.h
	gcc -Wall -pedantic -std=c99 cancel_subcontract.c \
                                        -lpthread -o cancel_subcontract.out

tsd_once: tsd_once.c errors.h
	gcc -Wall -pedantic -std=c99 tsd_once.c -lpthread -o tsd_once.out

tsd_destructor: tsd_destructor.c errors.h
	gcc -Wall -pedantic -std=c99 tsd_destructor.c \
										-lpthread -o tsd_destructor.out

sched_attr: sched_attr.c errors.h
	gcc -Wall -pedantic -std=c99 sched_attr.c \
										-lpthread -o sched_attr.out

sched_thread: sched_thread.c errors.h
	gcc -Wall -pedantic -std=c99 sched_thread.c \
										-lpthread -DDEBUG -o sched_thread.out