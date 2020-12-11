/*
Notes:
    Semaphores can be used instead of mutexes when
    the semaphore count is initalized to 1, where
    the semaphore will then exhibit a binary behavior.
    However, mutexes would still be a better option
    for short-lived problems over semaphores primarily
    due to the blocking operation done by semaphores would
    be unecessary for short-lived problems. 
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fifo.h"

void fifo_init(_FIFO *f)
{
    sem_init(&f->empty, MYFIFO_BUFSIZ);
    sem_init(&f->full, 0);
    f->w_index = 0;
    f->r_index = 0;
    f->lck = 0;
    memset(f->fifo_buf, 0, MYFIFO_BUFSIZ);
}


void fifo_wr(_FIFO *f, unsigned long d)
{
    sem_wait(&f->empty);
    spin_lock(&f->lck);
    d <<= 8;    //shift by 8bits to the left to hold process number
    d |= my_procnum;
    f->fifo_buf[f->w_index++] = d;
    f->w_index %= MYFIFO_BUFSIZ;    //for reseting the index once incrementing past buffer size bound
    sem_inc(&f->full);  //let consumer now read the updated data structure after writing
    spin_unlock(&f->lck);
}

unsigned long fifo_rd(_FIFO *f)
{
    sem_wait(&f->full);
    spin_lock(&f->lck);
    unsigned long data = f->fifo_buf[f->r_index++];
    f->r_index %= MYFIFO_BUFSIZ;
    sem_inc(&f->empty);  //let producer now write data to data structure after reading
    spin_unlock(&f->lck);
    return data;
}
