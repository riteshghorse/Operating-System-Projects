#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

int lcreate(void)
{
    STATWORD ps;
    int lock;
    disable(ps);
    
    if ((lock = newlock()) == (SYSERR)) {
        restore(ps);
        return(SYSERR);
    }    
    restore(ps);
    return(lock);
}

LOCAL int newlock()
{
    int i;
    int lock;
    for (i = 0; i < NLOCKS; ++i) {
        lock = nextlock--;
        if (nextlock < 0)
            nextlock = NLOCKS - 1;
        if (ltable[lock].lstate == LFREE) {
            ltable[lock].lstate = LUSED;
            ltable[lock].ltype = LFREE;
            return(lock);
        }
    }
    return(SYSERR);
}