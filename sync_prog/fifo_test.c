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
#include "fifo.h"

/*
    procnum = 0 => producer
    procnum = 1 => consumer
*/
int my_procnum = 0;

static int procnum[2] = {0, 1};
static struct fifo *f;

void* mem_creat(size_t);

int main(void)
{
    pid_t pid;
    srand(time(NULL));

    f = mem_creat(sizeof(*f));
    fifo_init(f);

    for (int i = 0; i < N_PROC; ++i)
    {
        switch (pid = fork())
        {
            case -1:
                fprintf(stderr, "Fork error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            case 0:
                if(i == 0)
                {
                    for(int j = 0; j < N_ITR; ++j)
                    {
                        unsigned long r_datum = fifo_rd(f);
                        f->r_data[i] = r_datum; 
                    }
                }
                else
                {
                    my_procnum = i;
                    for(int j = 0; j < N_ITR; ++j)
                    {
                        unsigned long w_datum = rand();
                        f->w_data[j] = w_datum;
                        fifo_wr(f, w_datum);
                    }
                }    
                exit(1);
        }
    }
    for(int p = 0; p < N_PROC; ++p)
    {
        if (wait(NULL) < 0)
        {
            fprintf(stderr, "Wait error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } 
    int diff_count = 0;
    fprintf(stderr, "FINAL:\n");
    for(int i = 0; i < N_ITR; ++i)
    {
        if(!(f->w_data[i] - f->r_data[i]))
            ++diff_count;
        //fprintf(stderr, "WRITTEN: %lu  |  READ: %lu\n", f->w_data[i], f->r_data[i]);
    }
    fprintf(stderr, "DIFF COUNT: %d\n", diff_count);
    if (munmap(f, sizeof(*f)) == -1)
    {
        fprintf(stderr, "Error unmapping virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

void* mem_creat(size_t bytes)
{
    void *addr;
    if ((addr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "Error generating virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return addr;
}
