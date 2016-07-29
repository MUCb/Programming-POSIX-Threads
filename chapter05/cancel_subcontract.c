#include <pthread.h>
#include "errors.h"

#define THREADS 5

/*
 * Struncture that defines the threads in a "team. "
 */
typedef struct team_tag {
    int         join_i;             /* join index */
    pthread_t   workers[THREADS];   /* thread identifiers */
} team_t;

/*
 * Start routine for worker threads. They loop wating for a cancellation 
 * request.
 */
void *worker_routine (void *arg)
{
    int counter;

    for (counter = 0; ; counter++)
        if ((counter % 1000) == 0)
            pthread_testcancel ();
}

/*
 * Cancellation cleanup handler for the contractor thread. It will cancel
 * and detach each worker in the team.
 */
void cleanup (void *arg)
{
    team_t *team = (team_t *)arg;
    int count, status;

    for (count = team->join_i; count < THREADS; count++) {
        status = pthread_cancel (team->workers[count]);
        if (status != 0)
            err_abort(status, "Cancel worker");

        status = pthread_detach (team->workers[count]);
        if (status != 0)
            err_abort(status, "Detach worker");
        printf("Cleanup: canceled %d\n", count);
    }
}

/*
 * Thread start routine for the contractor. It creates a team of worker threads,
 * and then joins with them. When canceled, the cleanup handler will cancel 
 * and detach the reamining threads.
 */
void *thread_routine (void *arg)
{
    team_t team;
    int count;
    void *result;
    int status;

    for (count = 0; count < THREADS; count++){
        status = pthread_create (
                            &team.workers[count], NULL, worker_routine, NULL);
        if (status != 0)
            err_abort(status, "Create worker");
    }
    pthread_cleanup_push (cleanup, (void *)&team);

    for (team.join_i = 0; team.join_i < THREADS; team.join_i++){
        status = pthread_join (team.workers[team.join_i], &result);
        if (status != 0)
            err_abort (status, "Join worker");
    }

    pthread_cleanup_pop (0);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread_id;
    int status;

#ifdef sun
    /*
     * On Solaris 2.5, threads are not timesliced. To ensure that our threads 
     * can run concurrently, we need to encrease the concurrency level 
     * to atleast 2 plus THREADS (the number of workers).
     */
    DPRINTF (("Setting concurrency level to %d\n", THREADS+2));
    thr_setconcurrency (THREADS+2);
#endif

    status = pthread_create (&thread_id, NULL, thread_routine, NULL);
    if (status != 0 )
        err_abort (status, "Create team");
    sleep (5);
    printf("Cancelling...\n");
    status = pthread_cancel (thread_id);
    if (status != 0)
        err_abort (status, "Cancel team");
    status = pthread_join (thread_id, NULL);
    if (status != 0)
        err_abort (status, "Join team");

    return 0;
}