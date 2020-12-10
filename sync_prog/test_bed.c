#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "spin_lock.h"

#define CPU_NUM 4

static struct mutex *mut;

void* mem_creat(size_t);
int proc_gen(int, int, char *);

int main(void)
{    
    pid_t pid;
    mut = mem_creat(sizeof(*mut));
    for(int i = 0; i < CPU_NUM; ++i)
    {
        switch(pid = fork())
        {
            case -1:
                fprintf(stderr, "Fork error: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            case 0:
                spin_lock(mut);
                for (int j = 0; j < 100000; j++)
                {
                    int prev_val = mut->data;
                    ++(mut->data);
                    fprintf(stderr, "VALUE: %d\n", mut->data);
                }
                spin_unlock(mut);
                exit(1);
        }        
    }
        
    for(int p = 0; p < CPU_NUM; ++p)
    {
        if (wait(NULL) < 0)
        {
            fprintf(stderr, "Wait error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "FINAL: %d\n", mut->data);
    if (munmap(mut, sizeof(*mut)) == -1)
    {
        fprintf(stderr, "Error unmapping virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

void* mem_creat(size_t bytes)
{
    void* addr;
    if((addr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    {
        fprintf(stderr, "Error generating virtual mem space: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return addr;
}
