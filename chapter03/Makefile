all: alarm_mutex backoff trylock mutex_static mutex_dynamic

alarm_mutex: alarm_mutex.c errors.h
	gcc -Wall -pedantic -std=c99 alarm_mutex.c -lpthread -o alarm_mutex.out

backoff: backoff.c errors.h
	gcc -Wall -pedantic -std=c99 backoff.c -lpthread -o backoff.out

trylock: trylock.c errors.h
	gcc -Wall -pedantic -std=c99 trylock.c -lpthread -o trylock.out

mutex_static: mutex_static.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_static.c -lpthread -o mutex_static.out

mutex_dynamic: mutex_dynamic.c errors.h
	gcc -Wall -pedantic -std=c99 mutex_dynamic.c -lpthread -o mutex_dynamic.out