#ifndef __SEM_H
#define __SEM_H
#include <sys/types.h>
#include "spin_lock.h"

#define N_PROC 64

extern int my_procnum;

typedef struct semaphore
{
    int rec_count; //number of resources available
    volatile char mtx; 
    pid_t waitlist[N_PROC]; //waitlist array    
    int b_count, w_count;
}_sem;

void sem_init(_sem *, int); 
int sem_try(_sem *);
void sem_wait(_sem *);
void sem_inc(_sem *);
#endif