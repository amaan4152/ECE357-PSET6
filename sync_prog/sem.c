#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include "spin_lock.h"
#include "sem.h"



//init operation
void sem_init(_sem *s, int count)
{
    s->rec_count = count;
    s->mtx = 0;
    memset(s->waitlist, 0, N_PROC);
}

void handler(int signum){}

//P oepration
void sem_wait(_sem *s)
{   
    sigset_t mask, old_mask; 
    for(;;)
    {
        spin_lock(&s->mtx);
        if(s->rec_count > 0)
        {
            s->waitlist[my_procnum] = 0;
            --s->rec_count;
            spin_unlock(&s->mtx);
            return;
        }
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask, &old_mask);
        s->waitlist[my_procnum] = getpid();
        //fprintf(stderr, "[P: %d] D00\n", my_procnum);
        spin_unlock(&s->mtx);
        signal(SIGUSR1, handler);
        sigemptyset(&mask);
        sigsuspend(&mask);   
    }
}

//V operation
void sem_inc(_sem *s)
{
    spin_lock(&s->mtx);
    //printf("STARTING INC! | PROC: %d\n", my_procnum);
    ++s->rec_count;
    for(int i = 0; i < N_PROC; ++i)
    {
        if(s->waitlist[i])
        {
            if(kill(s->waitlist[i], SIGUSR1) == -1)
            {
                fprintf(stderr, "Error sending signal to proc: %u\nERROR: %s\n", s->waitlist[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    spin_unlock(&s->mtx);
}

//TRY operation
int sem_try(_sem *s)
{
    spin_lock(&s->mtx);
    int test_count = s->rec_count;
    s->rec_count = (--test_count) >= 0 ? test_count : s->rec_count;
    spin_unlock(&s->mtx);
    return (test_count < 0);
}

