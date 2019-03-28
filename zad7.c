/*
*   Daniel Dubiel
*    291111
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "zad7.h"

synch s_data;
l_node list;

l_node *stack_init();

int main()
{
    list.val = -1;

    s_data.mut_add = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.mut_synch = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_data.append_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    s_data.rm_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    s_data.schr_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    pthread_t id[MAX_THREADS];
    for (int i = 1; i < MAX_THREADS - 1; i += 2)
    {
        pthread_create(&id[i - 1], NULL, reader, NULL);
        pthread_create(&id[i], NULL, writer, NULL);
    }
    printf("THREADS CREATED\n");
    for (int i = 0; i < MAX_THREADS; i++)
    {
        errno = pthread_join(id[i], NULL);
        perror("pthread_join");
    }
    return EXIT_SUCCESS;
}

void *reader()
{
    printf("READER CREATED\n");
    while (1)
    {
        usleep(rand() % 500);
        search(rand() % N);
    }
}

void *writer()
{
    printf("WRITER CREATED\n");
    l_node *stack = stack_init();
    while (1)
    {
        usleep(rand() % 1000);
        if (rand() % 3 == 0)
        {
            int x = rand() % N;
            push(stack, x);
            append_cond(x);
        }
        else
        {
            int x = pop(stack);
            remove_(x);
        }
    }
}

int search(int n)
{
    pthread_mutex_lock(&(s_data.mut_synch));
    while (s_data.rm_wait && s_data.rm_turn == 1)
        pthread_cond_wait(&(s_data.rm_cond), &(s_data.mut_synch));

    s_data.search_ops++;
    s_data.rm_turn = 1;
    pthread_mutex_unlock(&(s_data.mut_synch));

    int is_elem = 0;

    l_node *node = &list;
    while (node != NULL)
    {
        if (node->val == n)
        {
            is_elem = 1;
            break;
        }
        node = node->next;
    }

    pthread_mutex_lock(&(s_data.mut_synch));
    printf("SCH: %d, is_elem?: %d\n", n, is_elem);
    s_data.search_ops--;
    pthread_cond_broadcast(&(s_data.schr_cond));
    pthread_mutex_unlock(&(s_data.mut_synch));
    return is_elem;
}

void append_cond(int n)
{
    pthread_mutex_lock(&(s_data.mut_synch));
    while (s_data.rm_wait && s_data.rm_turn == 1)
        pthread_cond_wait(&(s_data.rm_cond), &(s_data.mut_synch));

    s_data.add_op++;
    s_data.rm_turn = 1;
    pthread_mutex_unlock(&(s_data.mut_synch));
    pthread_mutex_lock(&(s_data.mut_add));
    l_node *p_node = &list, *node = &list;
    while (node != NULL)
    {
        p_node = node;
        node = node->next;
    }

    l_node *new = (l_node *)malloc(sizeof(l_node));
    new->val = n;
    new->next = NULL;
    p_node->next = new;

    pthread_mutex_unlock(&(s_data.mut_add));
    pthread_mutex_lock(&(s_data.mut_synch));
    printf("ADD: %d\n", n);
    s_data.add_op--;
    pthread_cond_broadcast(&(s_data.append_cond));
    pthread_mutex_unlock(&(s_data.mut_synch));
}

void remove_(int n)
{
    if (n == -1)
        return;

    pthread_mutex_lock(&(s_data.mut_synch));
    s_data.rm_wait++;
    while (s_data.add_op)
    {
        pthread_cond_wait(&(s_data.append_cond), &(s_data.mut_synch));
    }
    while (s_data.search_ops)
    {
        pthread_cond_wait(&(s_data.schr_cond), &(s_data.mut_synch));
    }

    int is_elem = 0;

    l_node *node = &list;
    while (node != NULL)
    {
        if (node->next != NULL && node->next->val == n)
        {
            is_elem = 1;
            break;
        }
        node = node->next;
    }
    printf("RMV: %d, is_elem?: %d\n", n, is_elem);

    if (is_elem)
    {
        l_node *next = node->next->next;
        free(node->next);
        node->next = next;
    }

    s_data.rm_turn = 0;
    s_data.rm_wait--;
    pthread_cond_broadcast(&(s_data.rm_cond));
    pthread_mutex_unlock(&(s_data.mut_synch));
}

l_node *stack_init()
{
    l_node *stack = (l_node *)malloc(sizeof(l_node));
    stack->val = -1;
    stack->next = NULL;
    return stack;
}

void push(l_node *list, int n)
{
    l_node *node = list;
    while (node != NULL && node->next != NULL)
    {
        node = node->next;
    }

    node->next = (l_node *)malloc(sizeof(l_node));
    node->next->val = n;
    node->next->next = NULL;
}

int pop(l_node *list)
{
    int n;
    if (list == NULL || list->next == NULL)
        return -1;

    while (list->next->next != NULL)
    {
        list = list->next;
    }

    l_node *n_node = list->next->next;
    n = list->next->val;
    free(list->next);
    list->next = n_node;

    return n;
}