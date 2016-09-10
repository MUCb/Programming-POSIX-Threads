#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include "errors.h"

pid_t self_pid; 					/* pid of current process */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * This routine will be called prior to executing the fork,
 * with in the parrent process.
 */
void fork_prepare (void)
{
	int status;

	/*
	 * Lock the mutex in the parent before creating the child,
	 * to ensure that no other thread can lock it (or change any
	 * associated shared state) until after the fork completes.
	 */
	status = pthread_mutex_lock (&mutex);
	if (status != 0)
		err_abort (status, "Lock in prepare handler");
}

/*
 * This routine will be called after executing the fork, within
 * the parrent process
 */
void fork_parrent(void)
{
	int status;

	/*
	 * Unlock the mutex in the parent after the child has
	 * been created.
	 */
	status = pthread_mutex_unlock (&mutex);
	if (status != 0)
		err_abort (status, "Unlock in parent handler");
}

/*
 * This routine will be called after executing the fork, within
 * the child process.
 */
void fork_child (void)
{
	int status;

	/*
	 * Update the file scope "self_pid" within the child process, and
	 * unlock the mutex.
	 */
	self_pid = getpid();
	status = pthread_mutex_unlock (&mutex);
	if (status != 0)
		err_abort (status, "Unlock in child handler");
}

/*
 * Thread start routine, which will fork a new child process.
 */
void *thread_routine (void *arg)
{
	pid_t child_pid;
	int status;

	child_pid = fork();
	if (child_pid == (pid_t)-1)
		errno_abort("Fork");

	/*
	 * Lock the mutex -- without the atfork handlers, the mutex will 
	 * remain locked in the child process and this lock atempt will 
	 * hang (or fail with EDEADLK) in the child.
	 */
	status = pthread_mutex_lock (&mutex);
	if (status != 0)
		err_abort(status, "Lock in child");
	status = pthread_mutex_unlock (&mutex);
	if (status != 0)
		err_abort(status, "Unlock in child");
	printf ("After fork: %d (%d)\n", child_pid, self_pid);
	if (child_pid != 0){
		if ((pid_t)-1 == waitpid(child_pid, (int*)0, 0))
			errno_abort ("Wait for child");
	}
	return NULL;
}

int main (int argc, char *argv[])
{
	pthread_t fork_thread;
	int atfork_flag = 1;
	int status;

	if (argc > 1)
		atfork_flag = atoi (argv[1]);
	if (atfork_flag) {
		status = pthread_atfork (fork_prepare, fork_parrent, fork_child);
		if (status != 0)
			err_abort (status, "Register fork handlers");
	}
	self_pid = getpid();
	status = pthread_mutex_lock (&mutex);
	if (status != 0)
		err_abort (status, "Lock mutex");
	/*
	 * Create thread while mutex is locked. It will fork a
	 * process, which (without atfork handleers) will run 
	 * with hte mute locked.
	 */
	status = pthread_create (&fork_thread, NULL, thread_routine, NULL);
	if (status != 0)
		err_abort (status, "Create thread");
	sleep (5);
	status = pthread_mutex_unlock (&mutex);
	if (status != 0)
		err_abort (status, "Unlock mutex");
	status = pthread_join (fork_thread, NULL);
	if (status != 0)
		err_abort (status, "Join thread");
	return 0;
}