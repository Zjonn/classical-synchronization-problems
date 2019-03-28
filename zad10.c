/*
*   Daniel Dubiel
*    291111
*/
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "zad10.h"

synch s_data;

void *barber()
{
    while (1)
    {
        LOCK(s_data.customer);
        UNLOCK(s_data.barber);

        printf("C\n");

        LOCK(s_data.customer_done);
        UNLOCK(s_data.barber_done);
    }
}

void *customer()
{
    LOCK(s_data.mut);
    if (s_data.customers == SITS)
    {
        UNLOCK(s_data.mut);
        printf("L\n");
        LEAVE;
    }
    s_data.customers += 1;    
    UNLOCK(s_data.mut);

    UNLOCK(s_data.customer);   
    LOCK(s_data.barber);

    printf("C\n");

    UNLOCK(s_data.customer_done);
    LOCK(s_data.barber_done);

    LOCK(s_data.mut);
    s_data.customers -= 1;
    UNLOCK(s_data.mut);

    return 0;
}

int main()
{
    s_data.barber = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.barber_done = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.customer = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.customer_done = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.mut = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    pthread_t id[MAX_CUSTOMERS];

    pthread_create(&id[0], NULL, barber, NULL);

    for (int i = 1; i < MAX_CUSTOMERS; i++)
    {
        pthread_create(&id[i], NULL, customer, NULL);
    }
    printf("THREADS CREATED\n");
    for (int i = 1; i < MAX_CUSTOMERS; i++)
    {
        errno = pthread_join(id[i], NULL);
        //perror("pthread_join");
    }
    printf("All joined");
    return EXIT_SUCCESS;
}
