/*
*   Daniel Dubiel
*    291111
*/
#include <sys/types.h>
#include <sys/wait.h>

#define LOCK(X) (pthread_mutex_lock(&(X)))
#define UNLOCK(X) (pthread_mutex_unlock(&(X)))
#define LEAVE (pthread_exit(NULL))

typedef struct {
  pthread_mutex_t mut;

  pthread_mutex_t customer;
  pthread_mutex_t customer_done;

  pthread_mutex_t barber;
  pthread_mutex_t barber_done;

  int customers;
} synch;

#define MAX_CUSTOMERS 20
#define SITS 3

void *barber();
void *customer();
