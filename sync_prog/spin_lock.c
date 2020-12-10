#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "tas.h"
#include "spin_lock.h"

void spin_lock(struct mutex *m)
{
    while(tas(&m->spinlock))
    {
        //printf("SPINNING...\n");
        sched_yield();
    }
}

void spin_unlock(struct mutex *m)
{
    m->spinlock=0;
    //after unlock  
    //sched_yield();
}
