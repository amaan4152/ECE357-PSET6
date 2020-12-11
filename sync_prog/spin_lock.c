#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "tas.h"
#include "spin_lock.h"

void spin_lock(volatile char *lck)
{
    while(tas(lck)) sched_yield();
}

void spin_unlock(volatile char *lck)
{
    *lck=0;
}
