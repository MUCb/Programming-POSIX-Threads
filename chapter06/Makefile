all: atfork flock putchar getlogin susp

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
