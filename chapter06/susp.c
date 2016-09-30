#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "errors.h"

#define THREAD_COUNT    20
#define ITERATIONS      4000000

unsigned long thread_count = THREAD_COUNT;
unsigned long itertions = ITERATIONS;
pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
volatile int sentinel = 0;
pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_t *array = NULL, null_thread = {0};
int bottom = 0;
int inited = 0;

/*
 * Handle SIGUSR1 in the target thread, to suspend it untill 
 * reciving SIGUSR2 (resume).
 */
void suspend_signal_handler (int sig)
{
    sigset_t signal_set;

    /*
     * Block all the signals except SIGUSR2 while suspended.
     */
    sigfillset (&signal_set);
    sigdelset (&signal_set, SIGUSR2);
    sentinel = 1;
    sigsuspend(&signal_set);

    /*
     * Once I'm here, I've been resumed, and the resume signal
     * handler has been run to completion.
     */
    return;
}

/*
 * Handle SIGUSR2 in the target thread, to resume it. Note that
 * the signal handker does nothing. It exists only because we need
 * to cause sigsuspend() to return.
 */
void resume_signal_handler(int sig)
{
    return;
}

/*
 * Dynamically initialize the "suspend package" when first used
 * (called by pthread_once).
 */
void suspend_init_routine(void)
{
    int status;
    struct sigaction sigusr1, sigusr2;

    /*
     * Allocate the suspended threads array. This array is ussed
     * to guarantee idempotency.
     */
    bottom = 10;
    array = (pthread_t *) calloc (bottom, sizeof(pthread_t));

    /*
     * Install the signal handlers for suspend/resume.
     */
     sigusr1.sa_flags = 0;
     sigusr1.sa_handler = suspend_signal_handler;

     sigemptyset (&sigusr1.sa_mask);
     sigusr2.sa_flags = 0;
     sigusr2.sa_handler = resume_signal_handler;
     sigusr2.sa_mask = sigusr1.sa_mask;

     status = sigaction (SIGUSR1, &sigusr1, NULL);
     if (status == -1)
        errno_abort ("Installing suspend handler");

    status = sigaction (SIGUSR2, &sigusr2, NULL);
    if (status == -1)
        errno_abort ("Installing resume handler");
    
    inited = 1;
    return;
}

/*
 * Suspen a thread by sending it a signal (SIGUSR1), which will 
 * block the thread until anothre signal (SIGUSR2) arrives.
 *
 * Multiple calls to thd_suspend for a single thread have no
 * additional effect on the thread -- a single thd_continue
 * call will cause ut to resume execution.
 */
int thd_suspend (pthread_t target_thread)
{
    int status;
    int i = 0;

    /*
     * The first call to thd_suspend will initialize the package.
     */
    status = pthread_once (&once, suspend_init_routine);
    if (status != 0)
        return status;

    /*
     * Surialize access to suspend, makes life easier.
     */
    status = pthread_mutex_lock(&mut);
    if (status != 0)
        return status;

    /*
     * Threads that are suspended are added to the target_array;
     * a request to suspend a thread already listed in the array
     * is ignored. Sending e second SIGUSR1 would cause the 
     * thread to reuspend itself as soon as it is resumed.
     */
    while (i < bottom)
        if (array[i++] == target_thread) {
            status = pthread_mutex_unlock (&mut);
            return status;
        }

    /*
     * Ok,  we really need to suspend this thread. So, let's find
     * the location in the array that we'll use. If we run off
     * the end, realloc the array for the more space.
     */
    i = 0;
    while (array[i] != 0)
        i++;

    if (i == bottom){
        array = (pthread_t *) realloc (array, (++bottom * sizeof(pthread_t)));
        if (array == NULL){
            pthread_mutex_unlock (&mut);
            return errno;
        }

        array[bottom] = null_thread; /* Clear new entry */
    }

    /*
     * Clear the sentinel and signal the thread to suspend.
     */
    sentinel = 0;
    status = pthread_kill (target_thread, SIGUSR1);
    if (status != 0){
        pthread_mutex_unlock(&mut);
        return status;
    }

    /*
     * Wait for sentinel to chnange.
     */
    while (sentinel == 0)
        sched_yield();

    array[i] = target_thread;

    status = pthread_mutex_unlock(&mut);
    return status;
}

/*
 * Resume a suspended thread by sending a SIGUSR2 to break
 * it out of the sigsuspend() in whicj it's waiting. If the 
 * target thread isn't suspended, return with success.
 */
int thd_continue (pthread_t target_thread)
{
    int status;
    int i = 0;

    /*
     * Surialize access to suspend, make life easier.
     */
    status = pthread_mutex_lock (&mut);
    if (status != 0)
        return status;

    /*
     * If we haven't been initialized, then the thread must be
     * "resumed"; it couldn't have been suspended!
     */
    if (!inited) {
        status = pthread_mutex_unlock (&mut);
        return status;
    }

    /*
     * Make sure the thread is in the suspend array. If not, it
     * hasn't been suspended (or it has already been resumed) and
     * we can just carry on.
     */
    while (array[i] != target_thread && i < bottom)
        i++;

    if (i >= bottom) {
        pthread_mutex_unlock (&mut);
        return 0;
    }

    /*
     * Signal the thread to continue, and remove the thread from
     * the suspended array.
     */
    status = pthread_kill (target_thread, SIGUSR2);
    if (status != 0) {
        pthread_mutex_unlock (&mut);
        return status;
    }

    array[i] = 0;
    status = pthread_mutex_unlock (&mut);
    return status;
}

static void * thread_routine (void *arg)
{
    unsigned long number = (unsigned long  ) arg;
    int i;
    char buffer[128];

    for (i = 1; i <= itertions; i++) {
        /*
         * Every time each thread does 5000 iterations, print
         * a progress report.
         */
        if (i % 2000 == 0) {
            sprintf (buffer, "Thread %02lu: %d\n", number, i);
            write (1, buffer, strlen(buffer));
        }
        sched_yield ();
    }
    return (void *) 0;
}

int main (int argc, char * argv[])
{
    pthread_t threads[THREAD_COUNT];
    pthread_attr_t detach;
    int status;
    unsigned long i;

    status = pthread_attr_init (&detach);
    if (status != 0)
        err_abort (status, "Init attributes object");
    status = pthread_attr_setdetachstate (&detach, PTHREAD_CREATE_DETACHED);
    if (status != 0)
        err_abort (status, "Set create-detached");

    for (i = 0; i < THREAD_COUNT; i++){
        status = pthread_create (&threads[i], &detach, 
                                            thread_routine, (void *)i);
        if (status != 0)
            err_abort(status, "Create thread");
    }

    sleep(2);

    for ( i = 0; i < THREAD_COUNT/2; i++) {
        printf("Suspending thread %lu.\n", i);
        status = thd_suspend (threads[i]);
        if (status != 0)
            err_abort (status, "Suspend thread");
    }

    printf("Sleeping ...\n");
    sleep(2);

    for (i = 0; i < THREAD_COUNT/2; i++){
        printf("Continuing thread %lu.\n", i);
        status = thd_continue (threads[i]);
        if (status != 0)
            err_abort(status, "Continue thread");
    }

    for (i = THREAD_COUNT/2; i < THREAD_COUNT; i++){
        printf("Suspending the thread %lu.\n",i );
        status = thd_suspend (threads[i]);
        if (status != 0)
            err_abort (status, "Suspend thread");
    }

    printf("Sleeping ...\n");
    sleep(2);

    for (i = THREAD_COUNT/2; i < THREAD_COUNT; i++){
        printf("Continuing thread %lu.\n", i);
        status = thd_continue (threads[i]);
        if (status != 0)
            err_abort(status, "Continue thread");
    }
    pthread_exit (NULL); /* Let threads finish */
}