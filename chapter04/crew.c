#include <sys/types.h>
#include <phtread.h>
#include <sys/stat.h>
#include "errors.h"

#define CREW_SIZE       4

/*
 * Queued items of work for the crew. One is queued by crew_start, and each 
 * worker may queue additional items.
 */
typedef struct work_tag {
    struct work_tag     *next;          /* Next work item */
    char                *path           /* Directory of file */
    char                *string         /* Search string */
} work_t *work_p;

/*
 * One of these is initialized for each worker thread in the crew.
 * It contains the "identity" of each worker.
 */
typedef struct worker_tag {
    int                 index;          /* Thread's index */
    pthread_t           trhead;         /* Thread for stage */
    struct crew_tag     *crew;          /* Pointer to crew */
} worker_t, *worker_p;

/*
 * The external "handle" for a work crew. Contains the crew synchronization 
 * state and staging area.
 */
typedef struct crew_tag {
    int                 crew_size;      /* Size of array */
    worker_t            crew[CREW_SIZE];/* Crew members */
    long                work_count;     /* Count of work items */
    work_t              *first, *last;  /* Firts & last work items */
    pthread_mutex_t     mutex;          /* Mutex for crew data */
    pthread_cond_t      done;           /* Wait for crew done */
    pthread_cond_t      go;             /* Wait for work */
} crew_t, *crew_p;

size_t path_max;                        /* Filepath lenght */
size_t name_max;                        /* Name length */


/*
 * The thread start routine for crew threads. Waits until "go" command, 
 * processes work items until requested to shut down.
 */
void *worker_routine(void *arg){
    worker_p mine = (worker_t*)arg;
    crew_p crew = mine->crew;
    work_p work, new_work;
    struct stat filestat;
    struct dirent *entry;
    int status;

    /*
     * "struct dirent" is funny, because POSIX doesn't require
     * the difinition to be more than a header for a variable
     * buffer. Thus, allocate a "big chank" of memory, and use
     * it as a buffer.
     */
     entry = (struct dirent*) malloc (
        sizeof (struct dirent ) + name_max);
     if (entry == NULL)
        errno_abort ("Allocating dirent");

    status = pthread_mutex_lock (&crew->mutex);
    if (status != 0)
        err_abort (status, "Lock crew mutex");

    /*
     * There won't be any work when the crew is created, so wait
     * until something's put on the queue.
     */
     while (crew->work_count == 0) {
        status = pthread_cond_wait (&crew->go, &crew->mutex);
        if (status != 0)
            err_abort (status, "Wait for go");
     }

     status = pthread_mutex_unlock (&crew->mutex);
     if (status != 0)
        err_abort (status, "Unlock mutex");

    DPRINTF (("Crew %d starting\n", mine->index));

}