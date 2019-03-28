/*
*   Daniel Dubiel
*    291111
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LOCK(X) (pthread_mutex_lock(&(X)))
#define UNLOCK(X) (pthread_mutex_unlock(&(X)))
#define LEAVE (pthread_exit(NULL))

#define WAIT(COND, Y) (pthread_cond_wait(&(COND), &(Y)))
#define SIGNAL(COND)            \
    for (int i = 0; i < N; i++) \
    pthread_cond_signal(&(COND))

#define LOAD() (printf("LD\n"))
#define UNLOAD() (printf("UL\n"))
#define RUN() (printf("RN\n"))

#define BOARD() (printf("BD\n"))
#define UNBOARD() (printf("UB\n"))

typedef struct
{
    int boards;
    int unboards;
    pthread_cond_t board;
    pthread_cond_t unboard;

    pthread_mutex_t b_m;
    pthread_mutex_t ub_m;

    pthread_mutex_t all_aboard;
    pthread_mutex_t all_ashore;

    pthread_mutex_t mutex;
    pthread_mutex_t mutex2;
    int in_que;
} synch;

#define PASSENGERS 20
#define N 4

void *barber();
void *customer();
