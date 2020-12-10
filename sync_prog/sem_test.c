#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "sem.h"

/*
static struct semaphores
{
    _sem s_r;
    _sem s_w; 
}*s;
*/
static _sem *s;

void *mem_creat(size_t);

int main(void)
{
    pid_t pid;
    srand(time(NULL));

    s = mem_creat(sizeof(*s));
    sem_init(s, N_PROC);

    for (int i = 0; i < N_PROC; ++i)
    {
        switch (pid = fork())
        {
        case -1:
            fprintf(stderr, "Fork error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        case 0:

            for (unsigned long j = 0; j < 10000; ++j)
            {
                my_procnum = i;
                switch(i)
                {
                    case 0:                        
                        sem_wait(s);
                        break;
                    case 1:                        
                        sem_inc(s);  
                        break;                  
                }
            }
            exit(1);
        }
    }
    for (int p = 0; p < N_PROC; ++p)
    {
        if (wait(NULL) < 0)
        {
            fprintf(stderr, "Wait error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "FINAL:\n");
    for (int i = 0; i < 100; ++i)
    {
        fprintf(stderr, "BLOCK COUNT: %d | WAKE COUNT: %d | RESOURCE COUNT: %d\n", s->b_count, s->w_count, s->rec_count);
    }
    if(munmap(s, sizeof(*s)) == -1)
    {
        fprintf(stderr, "Error unmapping virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0; 
}

void *mem_creat(size_t bytes)
{
    void *addr;
    if ((addr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "Error generating virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return addr;
}
