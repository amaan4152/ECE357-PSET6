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
    s->b_count = 0;
    s->w_count = 0;
    s->mtx.spinlock = 0;
    memset(s->waitlist, 0, N_PROC);
}

void handler(int signum) //dummy handler
{
    fprintf(stderr, "~~~~~>IN SIGNAL HANDLER!\n");
    return;
}

//P oepration
void sem_wait(_sem *s)
{   
    //METHOD A
    sigset_t mask, mask_set;
    signal(SIGUSR1, handler);
    while(1)        
    {
        spin_lock(&s->mtx);
        sigemptyset(&mask_set);
        sigaddset(&mask_set, SIGUSR1);
        if(s->rec_count > 0)
        {
            s->waitlist[my_procnum] = 0;
            --s->rec_count;
            spin_unlock(&s->mtx);
            return;
        }
        
        ++s->b_count;
        sigprocmask(SIG_BLOCK, &mask_set, &mask);
        s->waitlist[my_procnum] = getpid();
        fprintf(stderr, "D00\n");
        sigemptyset(&mask);
        spin_unlock(&s->mtx);
        sigsuspend(&mask);
        
        fprintf(stderr, "D01 | PID: %u RECIEVED SIGUSR1 | PROC_NUM: %d\n", getpid(), my_procnum);
    }
}

//V operation
void sem_inc(_sem *s)
{
    spin_lock(&s->mtx);
    //printf("STARTING INC! | PROC: %d\n", my_procnum);
    for(int i = 0; i < N_PROC; ++i)
    {
        if(s->waitlist[i])
        {
            //fprintf(stderr, "KILLING TO PROCESS: %u!\n", s->waitlist[i]); //debug statement
            if(kill(s->waitlist[i], SIGUSR1) == -1)
            {
                fprintf(stderr, "Error sending signal to proc: %u\nERROR: %s\n", s->waitlist[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
            ++s->rec_count;
            ++s->w_count;
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

