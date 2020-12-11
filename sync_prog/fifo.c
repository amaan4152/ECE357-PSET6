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

/*
    If the buffer is full, we want to block 
    all producer (writer) operations till 
    the item_count < MYFIFO_BUFSIZ

    In order to preserve the fifo struct whilst 
    enqueue operation is taking place, we must prevent
    any other producer (writer) performing the same operation
    to the fifo data structure, as well as prevent 
    any consumer from performing any dequeue oepration 
    to the fifo data structure. Thus, we must perform
    a mutex lock on the fifo structure. 
*/
void fifo_wr(_FIFO *f, unsigned long d)
{
    //spin_lock(&f->f_mtx);
    sem_wait(&f->empty);
    spin_lock(&f->lck);
    //fprintf(stderr, "[P%d] EMPTY\n", my_procnum);
    f->fifo_buf[f->w_index++] = d;
    f->w_index %= MYFIFO_BUFSIZ;    //for reseting the index once incrementing past buffer size bound
    spin_unlock(&f->lck);
    sem_inc(&f->full);  //let consumer now read the updated data structure after writing
    //fprintf(stderr, "[P%d] ADDED DATA\n", my_procnum);
}

unsigned long fifo_rd(_FIFO *f)
{
    //spin_lock(&f->f_mtx);
    sem_wait(&f->full);
    spin_lock(&f->lck);
    //fprintf(stderr, "[P%d] FULL\n", my_procnum);
    unsigned long data = f->fifo_buf[f->r_index++];
    f->r_index %= MYFIFO_BUFSIZ;
    spin_unlock(&f->lck);
    sem_inc(&f->empty);  //let producer now write data to data structure after reading
    //fprintf(stderr, "[P%d] READ DATA\n", my_procnum);
    //printf("DATA: %lu\n", data);
    return data;
}
