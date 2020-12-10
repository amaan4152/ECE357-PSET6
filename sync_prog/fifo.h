#ifndef __FIFO_H
#define __FIFO_H
#include "sem.h"

#define MYFIFO_BUFSIZ 4096
#define N_ITR 100

typedef struct fifo
{
    _sem cons; //semaphore for consumer process (reader)
    _sem prod; //semapore for producer process (writer)
    struct mutex f_mtx;
    unsigned long fifo_buf[MYFIFO_BUFSIZ], r_data[N_ITR], w_data[N_ITR];
    int item_count, w_index, r_index; 
}_FIFO;

void fifo_init(_FIFO *);
void fifo_wr(_FIFO *, unsigned long);
unsigned long fifo_rd(_FIFO *);
#endif