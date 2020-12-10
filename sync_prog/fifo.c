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
#include "fifo.h"

void fifo_init(_FIFO *f)
{
    sem_init(&f->cons, 1);
    sem_init(&f->prod, 9);
    f->item_count = 0; 
    f->w_index = 0;
    f->r_index = 0;
    f->f_mtx.spinlock = 0;
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
    if(f->item_count >= MYFIFO_BUFSIZ)   //full
    {
        //printf("FULL!\n");
        sem_wait(&f->prod); //block producer till data struct has a free slot
    }
    spin_lock(&f->f_mtx);
    f->fifo_buf[f->w_index++] = d;
    f->w_index %= MYFIFO_BUFSIZ;    //for reseting the index once incrementing past buffer size bound
    ++f->item_count;
    spin_unlock(&f->f_mtx);
    sem_inc(&f->cons);  //let consumer now read the updated data structure after writing
}

unsigned long fifo_rd(_FIFO *f)
{
    unsigned long data;
    if(f->item_count <= 0)  //empty
    {
        //printf("EMPTY!\n");
        sem_wait(&f->cons); //block consumer till producer has written something to data structure
    }
    spin_lock(&f->f_mtx);
    data = f->fifo_buf[f->r_index++];
    f->r_index %= MYFIFO_BUFSIZ;
    --f->item_count;
    spin_unlock(&f->f_mtx);
    sem_inc(&f->prod);  //let producer now write data to data structure after reading
    //printf("DATA: %lu\n", data);
    return data;
}
