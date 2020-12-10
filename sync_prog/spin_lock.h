#ifndef __SPIN_LCK_H
#define __SPIN_LCK_H
struct mutex
{
    char spinlock; 
};
void spin_lock(struct mutex *);
void spin_unlock(struct mutex *);
#endif 
