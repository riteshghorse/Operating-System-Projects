#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int ldelete(int ldes)
{
    STATWORD ps;
    struct lentry *lptr;
    int pid;
    disable (ps);
    if (isbadlock(ldes) || ltable[ldes].lstate == LFREE) {
        restore (ps);
        return(SYSERR);
    }

    lptr = &ltable[ldes];
    lptr->lstate = LFREE;
    lptr->lockcnt = 0;
    lptr->ltype = LFREE;
    if (nonempty(lptr->lqhead)) {
		while ( (pid=getfirst(lptr->lqhead)) != EMPTY) {
		    proctab[pid].lwaitret = DELETED;
		    ready(pid,RESCHNO);
        }
		resched();
	}
    restore (ps);
    return(DELETED);
}