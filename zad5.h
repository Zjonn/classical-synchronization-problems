/*
*   Daniel Dubiel
*    291111
*/
#include <unistd.h>
#include <signal.h>

#define KBLU "\x1B[34m"
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"

#define SEM_NAME_1 "/zad5_mutex"
#define SEM_NAME_2 "/zad5_sem_eat"

#define wait_with_perror(X)                        \
    do                                             \
    {                                              \
        if (sem_wait(X) < 0)                       \
        {                                          \
            perror("sem_wait(3) failed on child"); \
        }                                          \
    } while (0)

#define post_with_perror(X)                        \
    do                                             \
    {                                              \
        if (sem_post(X) < 0)                       \
        {                                          \
            perror("sem_wait(3) failed on child"); \
        }                                          \
    } while (0)

typedef struct
{
    int waiting;
    int must_wait;
    int eating;
} shared_data;

shared_data *data;

void client();

inline void enter(sem_t *, sem_t *);
inline void leave(sem_t *, sem_t *);

void clean();

#define CLIENTS 50
#define EATING_TIME 1