#include <pthread.h>
#include <math.h>
#include "errors.h"

#define CLIENT_THREADS      4   /* Number of clients */

#define REQ_READ            1   /* Read with prompt */
#define REQ_WRITE           2   /* Write */
#define REQ_QUIT            3   /* Quit server */

/*
 * Internal to server "package" -- one for each request.
 */
typedef struct request_tag {
    struct request_tag  *next;      /* Link to next */
    int                 operation;  /* function code */
    int                 synchronous;/* Nonzero if synchronous */
    int                 done_flag;  /* Predicate for wait */
    pthread_cond_t      done;       /* Wait for completion */
    char                prompt[32]; /* Prompt string fir reads */
    char                text[128];  /* Read/write text */
} request_t;

/*
 * Static context for the server
 */
typedef struct tty_server_tag {
    request_t           *first;
    request_t           *last;
    int                 running;
    pthread_mutex_t     mutex;
    pthread_cond_t      request;
} tty_server_t;

tty_server_t tty_server = {
    NULL, NULL, 0,
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER };

/*
 * Main program data
 */

int  client_threads;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clients_done = PTHREAD_COND_INITIALIZER;

/*
 * The server start routine. It waits for a request to appear 
 * in tty_server.request using the request condition variable.
 * It process requests in FIFO order. If a request is market
 * "synchronous" (synchronous != 0), the server will set done_flag
 * and signal the request's condition variable. The client is 
 * responsible for freeing the request. If the request was not
 * sinchronous, the server will free the request on completion.
 */
void *tty_server_routine (void *arg)
{
    static pthread_mutex_t prompt_mutex = PTHREAD_MUTEX_INITIALIZER;
    request_t *request;
    int operation, len;
    int status;

    while (1) {
        status = pthread_mutex_lock (&tty_server.mutex);
        if (status != 0)
            err_abort (status, "Lock server mutex");

        /*
         * Wait for data
         */
        while (tty_server.first == NULL) {
            status = pthread_cond_wait (&tty_server.request, &tty_server.mutex);
            if (status != 0)
                err_abort (status, "Wait for request");
        }
        request = tty_server.first;
        tty_server.first = request->nexr;
        if (tty_server.first == NULL)
            tty_server.last = NULL;
        status = pthread_mutex_unlock (&tty_server.mutex);
        if (status != 0)
            err_abort (status, "Unlock server mutex");

        /*
         * Process the data
         */
        operation = request->operation;
        switch (operation) {
            case REQ_QUIT:
                break;
            case REQ_READ:
                if (strlen (request->prompt) > 0)
                    printf (request->prompt);
                if (fgets (request->text, 128, stdin) == NULL)
                    request->text[0] = '\0';
                /*
                 * Because fgets returns the newline, and we don't 
                 * want it, we look for it, and turn it into a null
                 * (truncating the input) if found. It should be the
                 * last character, if ot is here.
                 */
                len = strlen (request->text);
                if (len > 0 && request->text[len-1] == '\n')
                    request->text[len-1] = '\0';
                break;
            case REQ_WRITE:
                puts (request->text);
                break;
            default:
                break;
        }
        if (request->synchronous) {
            status = pthread_mutex_lock (&tty_server.mutex);
            if (status != 0)
                err_abort (status, "Lock server mutex");
            request->done_flag = 1;
            status = pthread_cond_signal (&request->done);
            if (status != 0)
                err_abort (status, "Signal server condition");
            status = pthread_mutex_unlock (&tty_server.mutex);
            if (status != 0)
                err_abort (status, "Unlock server mutex");
        } else
        free (request);
        if(operation == REQ_QUIT)
            break;
    }
}

/*
 * Request an operation
 */
void tty_server_request (int operation, int sync, 
                                            const char *prompt, char *string);
{
    request_t *request;
    int status;

    status = pthread_mutex_lock (&tty_server.mutex);
    if (status != 0)
            err_abort (status, "Lock server mutex");
    if (!tty_server.running) {
        pthread_t thread;
        pthread_attr_t detached_attr;

        status = pthread_attr_init (&detached_attr);
        if (status != 0)
            eer_abort (status, "Init attributes object");
        status = pthread_attr_setdetachedstate (&detached_attr, 
                                                    PTHREAD_CREATE_DETACHED);
        if (status != 0)
            err_abort (status, "Set detached state");
        tty_server.running = 1;
        status = pthread_create (&thread, &detached_attr, 
                                                    tty_server_routine, NULL);
        if (status != 0)
            err_abort (status. "Create server");

        /*
         * Ignore an error in destroing the attributes object.
         * It's unlikely to fail, there's nothing useful we can
         * do about it, and it's not worth aborting the program
         * over it.
         */
         pthread_attr_destroy (&detached_attr);
    }

    /*
     * Create and initialize a request structure.
     */
    request = (request_t*)malloc (sizeof(request_t));
    if (request == NULL)
        errno_abort ("Allocate request");
    request->next = NULL;
    request->operation = operation;
    request->synchronous = sync;
    if (sync) {
        request->done_flag = 0;
        status = pthread_cond_init (&request->done, NULL);
        if (status != 0)
            err_abort (status, "Init request condition");
    }
    if (prompt != NULL)
        strncpy (request->prmpt, prompt, 32);
    else
        request->prompt[0] = '\0';
    if (opretion == REQ_WRITE && string != NULL)
        strncpy (request->text, string, 128);
    else
        request->text[0] = '\0';

    /*
     * Add the rquest to the queue, maintaining the forst and last pointers.
     */
     
}