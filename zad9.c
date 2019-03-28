/*
*   Daniel Dubiel
*    291111
*/

#include "zad9.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

synch s_data;

void *car()
{
    //I waiting for pasangers to come
    sleep(1);
    while (1)
    {
        LOAD();
        SIGNAL(s_data.board);
        LOCK(s_data.all_aboard);

        RUN();

        UNLOAD();
        SIGNAL(s_data.unboard);
        LOCK(s_data.all_ashore);
    }
}

void *passenger()
{
    LOCK(s_data.b_m);
    WAIT(s_data.board, s_data.b_m);
    UNLOCK(s_data.b_m);

    LOCK(s_data.mutex);
    BOARD();
    s_data.boards += 1;
    if (s_data.boards == N)
    {
        UNLOCK(s_data.all_aboard);
        s_data.boards = 0;
    }
    UNLOCK(s_data.mutex);

    LOCK(s_data.ub_m);
    WAIT(s_data.unboard, s_data.ub_m);
    UNLOCK(s_data.ub_m);

    LOCK(s_data.mutex2);
    UNBOARD();
    s_data.unboards += 1;
    if (s_data.unboards == N)
    {
        UNLOCK(s_data.all_ashore);
        s_data.unboards = 0;
    }
    UNLOCK(s_data.mutex2);

    return 0;
}

int main()
{
    s_data.all_aboard = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.all_ashore = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    s_data.board = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    s_data.unboard = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    s_data.b_m = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.ub_m = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    s_data.mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.mutex2 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    LOCK(s_data.all_aboard);
    LOCK(s_data.all_ashore);

    pthread_t id[PASSENGERS + 1];
    pthread_create(&id[0], NULL, car, NULL);

    for (int i = 1; i < PASSENGERS + 1; i++)
    {
        pthread_create(&id[i], NULL, passenger, NULL);
    }
    printf("THREADS CREATED\n");
    for (int i = 1; i < PASSENGERS; i++)
    {
        errno = pthread_join(id[i], NULL);
        // perror("pthread_join");
    }
    printf("All joined");
    return EXIT_SUCCESS;
}