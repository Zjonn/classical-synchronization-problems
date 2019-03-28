/*
*   Daniel Dubiel
*    291111
*/
#include "zad6.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

// VS code intelisense fix
#ifndef __USE_MISC
#define __USE_MISC
#endif
#include <sys/mman.h>

void savage()
{
    sem_t *empty_pot = sem_open(SEM_NAME_1, O_RDWR);
    if (empty_pot == SEM_FAILED)
    {
        perror("sem_open(3):empty client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *full_pot = sem_open(SEM_NAME_2, O_RDWR);
    if (full_pot == SEM_FAILED)
    {
        perror("sem_open(3):full client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *mutex = sem_open(SEM_NAME_3, O_RDWR);
    if (full_pot == SEM_FAILED)
    {
        perror("sem_open(3):mut client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *servings = sem_open(SEM_NAME_4, O_RDWR);
    if (full_pot == SEM_FAILED)
    {
        perror("sem_open(3):mut client failed");
        exit(EXIT_FAILURE);
    }

    int id = getpid();
    while (1)
    {
        wait_with_perror(mutex);
        printf("%d WANT FOOD\n", id);
        int m;
        sem_getvalue(servings, &m);
        if (m == 1)
        {
            post_with_perror(empty_pot);
            wait_with_perror(full_pot);
            sem_getvalue(servings, &m);
        }
        wait_with_perror(servings);
        post_with_perror(mutex);
        printf("%d OMNOMNOM\n", id);
        //sleep(1);
    }
}

void cook()
{
    sem_t *empty_pot = sem_open(SEM_NAME_1, O_RDWR);
    if (empty_pot == SEM_FAILED)
    {
        perror("sem_open(3):empty client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *full_pot = sem_open(SEM_NAME_2, O_RDWR);
    if (full_pot == SEM_FAILED)
    {
        perror("sem_open(3):full client failed");
        exit(EXIT_FAILURE);
    }
    sem_t *servings = sem_open(SEM_NAME_4, O_RDWR);
    if (full_pot == SEM_FAILED)
    {
        perror("sem_open(3):mut client failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        wait_with_perror(empty_pot);
        printf("COOKING TIME!\n");
        for (int i = 0; i < MAX_SERVINGS; i++)
        {
            post_with_perror(servings);
            printf("Meal nr: %d\n", i + 1);
        }
        post_with_perror(full_pot);
    }
}

int main()
{
    sem_t *empty_pot = sem_open(SEM_NAME_1, O_CREAT | O_EXCL,
                                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
    sem_t *full_pot = sem_open(SEM_NAME_2, O_CREAT | O_EXCL,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
    sem_t *mutex = sem_open(SEM_NAME_3, O_CREAT | O_EXCL,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    sem_t *servings = sem_open(SEM_NAME_4, O_CREAT | O_EXCL,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);

    if (empty_pot == SEM_FAILED)
    {
        perror("sem_open(3):empty failed");
        exit(EXIT_FAILURE);
    }
    if (full_pot == SEM_FAILED)
    {
        sem_unlink(SEM_NAME_1);
        perror("sem_open(3):full failed");
        exit(EXIT_FAILURE);
    }
    if (mutex == SEM_FAILED)
    {
        sem_unlink(SEM_NAME_1);
        sem_unlink(SEM_NAME_2);
        perror("sem_open(3):mut failed");
        exit(EXIT_FAILURE);
    }
    if (servings == SEM_FAILED)
    {
        sem_unlink(SEM_NAME_1);
        sem_unlink(SEM_NAME_2);
        sem_unlink(SEM_NAME_3);
        perror("sem_open(3):mut failed");
        exit(EXIT_FAILURE);
    }
    if (sem_close(empty_pot) < 0 || sem_close(full_pot) < 0 ||
        sem_close(mutex) < 0 || sem_close(servings) < 0)
    {
        perror("sem_close(3) failed");
        clean();
        exit(EXIT_FAILURE);
    }

    create_cook();
    create_savages();

    signal(SIGINT, &clean);

    for (int i = 0; i < SAVAGES + 1; i++)
    {
        wait(NULL);
    }
    clean();
}

void create_cook()
{
    int pid = fork();
    if (pid == 0)
    {
        cook();
        exit(EXIT_SUCCESS);
    }
    else if (pid < 0)
        perror("fork() failed");
}

void create_savages()
{
    for (int i = 0; i < SAVAGES; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            savage();
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0)
            perror("fork() failed");
    }
}

void clean()
{
    if (sem_unlink(SEM_NAME_1) < 0 || sem_unlink(SEM_NAME_2) < 0 ||
        sem_unlink(SEM_NAME_3) < 0 || sem_unlink(SEM_NAME_4) < 0)
        perror("sem_unlink(3) failed");

    exit(EXIT_FAILURE);
}