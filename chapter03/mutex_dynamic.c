#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct my_struct_tag {
    pthread_mutex_t	mutex;
    int 		value;
}	my_struct_t;

int main (int argc, char *argv[])
{
    my_struct_t *data;
    int status;

    data = malloc(sizeof(my_struct_t));
    if (data == NULL)
    {
	perror("Allocate structure");
	return 1;
    }
    status = pthread_mutex_init(&data->mutex, NULL);
    if(status != 0)
    {
	perror("error init mutex");
	free(data);
	return 1;
    }
    status = pthread_mutex_destroy(&data->mutex);
    if( status != 0)
    {
	perror("error destroy mutex");
	(void) free(data);
	return 1;
    }
    (void) free(data);
    return 0;
}