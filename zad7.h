/*
*   Daniel Dubiel
*    291111
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

typedef struct _l_node
{
    int val;
    struct _l_node *next;
} l_node;

typedef struct
{
    pthread_mutex_t mut_synch;
    pthread_mutex_t mut_add;

    pthread_cond_t rm_cond;
    pthread_cond_t append_cond;
    pthread_cond_t schr_cond;

    int rm_wait;
    int rm_turn;
    int search_ops;
    int add_op;
} synch;

#define MAX_THREADS 10
#define N 1000000

void *reader();
void *writer();

int search(int n);
void append_cond(int n);
void remove_(int n);

void push(l_node *, int);
int pop(l_node *);