/*
*   Daniel Dubiel
*    291111
*/
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// VS code intelisense fix
#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <sys/mman.h>

#include "zad5.h"


int main()
{
    sem_t *mutex = sem_open(SEM_NAME_1, O_CREAT | O_EXCL,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    sem_t *sem_1 = sem_open(SEM_NAME_2, O_CREAT | O_EXCL,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 4);
    data = mmap(NULL, sizeof(data), PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_SHARED, 0, 0);

    if (mutex == SEM_FAILED)
    {
        perror("sem_open(3):m failed");
        exit(EXIT_FAILURE);
    }
    if (sem_1 == SEM_FAILED)
    {
        sem_unlink(SEM_NAME_1);
        perror("sem_open(3):s failed");
        exit(EXIT_FAILURE);
    }

    if (sem_close(mutex) < 0 || sem_close(sem_1) < 0)
    {
        perror("sem_close(3) failed");
        sem_unlink(SEM_NAME_1);
        sem_unlink(SEM_NAME_2);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < CLIENTS; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            client();
            return EXIT_SUCCESS;
        }
        else if (pid < 0)
            perror("fork() failed");
    }

    signal(SIGINT, &clean);

    for (int i = 0; i < CLIENTS; i++)
    {
        wait(NULL);
    }
    clean();
}

void client()
{
    sem_t *mutex = sem_open(SEM_NAME_1, O_RDWR);
    if (mutex == SEM_FAILED)
    {
        perror("sem_open(3):m client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *sem = sem_open(SEM_NAME_2, O_RDWR);
    if (sem == SEM_FAILED)
    {
        perror("sem_open(3):s client failed");
        exit(EXIT_FAILURE);
    }

    enter(mutex, sem);

    usleep(rand() % 3);

    leave(mutex, sem);
}

inline void enter(sem_t *mutex, sem_t *sem)
{
    wait_with_perror(mutex);

    if (data->must_wait)
    {
        ++(data->waiting);
        printf("%d waiting\n", data->waiting);
    
        while (data->must_wait)
        {
            post_with_perror(mutex);
            wait_with_perror(mutex);
        }
        wait_with_perror(sem);
        --(data->waiting);
        printf("PID: %d can eat. %d still waiting\n", getpid(), data->waiting);
    }
    else
        wait_with_perror(sem);

    ++(data->eating);
    if (data->eating == 4)
    {
        data->must_wait = 1;
        printf("PID: %d eates. %sEveryone must wait%s\n", getpid(), KRED, KNRM);
    }
    else
        printf("PID: %d eates\n", getpid());
    post_with_perror(mutex);
}

inline void leave(sem_t *mutex, sem_t *sem)
{
    wait_with_perror(mutex);
    --(data->eating);

    if (data->must_wait && data->eating == 0)
    {
        printf("PID: %d done. %sNext 5 can go%s\n", getpid(), KBLU, KNRM);
        data->must_wait = 0;
    }
    else
    {
        printf("PID: %d done. %d clients still eat\n", getpid(), data->eating);
    }
    post_with_perror(sem);
    post_with_perror(mutex);
}

void clean()
{
    if (sem_unlink(SEM_NAME_1) < 0 || sem_unlink(SEM_NAME_2) < 0)
        perror("sem_unlink(3) failed");

    exit(EXIT_FAILURE);
}
