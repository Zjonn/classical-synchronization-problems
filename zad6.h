/*
*   Daniel Dubiel
*    291111
*/
#include <unistd.h>
#include <signal.h>

#define SEM_NAME_1 "/empty_pot"
#define SEM_NAME_2 "/full_pot"
#define SEM_NAME_3 "/mutex_6"
#define SEM_NAME_4 "/pot_6"

#define SAVAGES 1000
#define MAX_SERVINGS 10

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

void create_cook();
void create_savages();
void clean();