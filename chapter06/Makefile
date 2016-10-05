all: atfork flock putchar getlogin susp sigwait sigev_thread semaphore_signal

atfork: atfork.c errors.h
	gcc -Wall -pedantic -std=c99 atfork.c  -lpthread -o atfork.out

flock: flock.c errors.h
	gcc -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE flock.c  -lpthread -o flock.out 

putchar: putchar.c errors.h
	gcc -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE putchar.c  -lpthread -o putchar.out 

getlogin: getlogin.c errors.h
	gcc -Wall -pedantic -std=c99 -D_POSIX_C_SOURCE -D_REENTRANT getlogin.c  -lpthread -o getlogin.out 

susp: susp.c errors.h
	gcc -Wall -pedantic  -lpthread  -std=c99 -D_POSIX_C_SOURCE=199506L susp.c -o susp.out 

sigwait: sigwait.c errors.h
	gcc -Wall -pedantic  -lpthread  -std=c99 -D_POSIX_C_SOURCE=199506L sigwait.c -o sigwait.out 

sigev_thread: sigev_thread.c errors.h
	gcc -Wall -pedantic  -lpthread -lrt -std=c99 -DDEBUG -D_POSIX_C_SOURCE=199506L sigev_thread.c -o sigev_thread.out 


semaphore_signal: semaphore_signal.c errors.h
	gcc -Wall -pedantic  -lpthread -lrt -std=c99 -DDEBUG -D_POSIX_C_SOURCE=199506L semaphore_signal.c -o semaphore_signal.out 
