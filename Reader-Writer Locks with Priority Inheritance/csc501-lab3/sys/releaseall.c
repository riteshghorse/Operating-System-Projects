#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

void get_lock(int ldes1) 
{
    struct lentry *lptr;
    int newproc, newprio;
    lptr = &ltable[ldes1];

    if (isbadpid(q[lptr->lqtail].qprev) EMPTY) {
        kprintf("empty queue\n");
        lptr->ltype = LFREE;
        lptr->lstate = LFREE;
        return;
    } else  {
        newproc = q[lptr->lqtail].qprev;
        newprio = q[newproc].qkey;
        kprintf("release -> %d : %d\n", newproc, newprio);
        /*include check for time when many write requests*/
        newproc = dequeue(newproc);
        ready(newproc, RESCHNO);
        lptr->ltype = lptr->lqtype[newproc];
        lptr->lockcnt = 1;
        lptr->lprio = newprio;
        // proctab[newproc].plock[ldes1] = 1;
        proctab[newproc].lwaitret = (OK);
        newproc = q[lptr->lqtail].qprev;
        newprio = q[newproc].qkey;
        if (lptr->ltype == READ) {	
			while (newproc != lptr->lqhead && 
                    lptr->lqtype[newproc] == READ &&
                    newprio > lptr->lprio) {	
				newproc = dequeue(newproc);
				ready(newproc, RESCHNO);
				lptr->lockcnt += 1;
				lptr->lqwait[newproc] = BADPID;
                lptr->lqtype[newproc] = LFREE;
				// proctab[newproc].plock[ldes1] = 1;
				proctab[newproc].lwaitret = (OK);
                newproc = q[newproc].qprev;
				newprio = q[newproc].qkey;
			}
		}
    }
}


int releaseall(int numlocks, int ldes1,...) 
{
    STATWORD ps;
    int i, currlock, res;
    struct lentry *lptr;
    disable (ps);
    res = (OK);
    for (i = 0; i < numlocks; ++i) {
        currlock = *(&ldes1 + i);
        lptr = &ltable[currlock];
        if (isbadlock(currlock) || 
            ltable[currlock].lstate == LFREE) {
            res = (SYSERR);
            continue;
        } else if (ltable[currlock].ltype == READ) {
            lptr->lockcnt -= 1;
            // proctab[currpid].plock[currlock] = LFREE;
            // lptr->lqwait[currpid] = BADPID;
            // lptr->lqtype[currpid] = LFREE;
            if (lptr->lockcnt == 0) {
                get_lock (currlock);
            }
        } else {    /* write */
            lptr->lockcnt = 0;
            lptr->lprio = 0;
            // lptr->lstate = LFREE;
            // lptr->ltype = LFREE;
            // lptr->lqwait[currpid] = BADPID;
            // lptr->lqtype[currpid] = LFREE;
            get_lock (currlock);
        }
    }
    restore (ps);
    return(res);
}