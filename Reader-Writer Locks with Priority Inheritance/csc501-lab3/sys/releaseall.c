#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int scanwaittime(int procid, int priority, int waittime, int ldes) 
{
    if (ltable[ldes].lqtype[procid] == WRITE) {
        return procid;
    }
    while(!isbadpid(q[procid].qprev) && q[q[procid].qprev].qkey == priority) {
        if (ltable[ldes].lqtype[q[procid].qprev] == WRITE &&
            (proctab[q[procid].qprev].lqwaittime - waittime) < 1000) {
                return (q[procid].qprev);
        }
        procid = q[procid].qprev;
    }
    return procid;
}

void get_lock(int ldes1) 
{
    struct lentry *lptr;
    int newproc, newprio;
    lptr = &ltable[ldes1];

    if (q[lptr->lqhead].qnext == lptr->lqtail) {
        kprintf("empty queue\n");
        lptr->ltype = LFREE;
        // lptr->lstate = LFREE;
        return;
    } else  {
        newproc = q[lptr->lqtail].qprev;
        newprio = q[newproc].qkey;
        // kprintf("release -> %d : %d\n", newproc, newprio);
        /*include check for time when many write requests*/
        newproc = scanwaittime(newproc, newprio, proctab[newproc].lqwaittime, ldes1);
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

/**
 * update max pprio of lock for
 * processes in wait queue
 */
void updatemaxprio (int ldes)
{
    struct lentry *lptr;
    int maxprio = 0;
    int i, curr;
    lptr = &ltable[ldes];
    if (isempty(lptr->lqhead)) {
        lptr->lpprio = -1;
        return;
    }
    curr = q[lptr->lqtail].qprev;
    while (curr != lptr->lqhead) {
        if (maxprio < proctab[curr].pprio)
            maxprio = proctab[curr].pprio;
        curr = q[curr].qprev;
    }
    // maxprio = max(maxprio, q[q[lptr->lqtail].qprev].qkey);
    lptr->lpprio = maxprio;
    kprintf("in maxprio %d\n", maxprio);
} 

/**
 * update pprio of processes holding the lock
 * 
 * */
int gethighprio(int ldes)
{
    int i;
    struct lentry *lptr;

    lptr = &ltable[ldes];
    for (i = 0; i < NPROC; ++i) {
        if (lptr->lholdprocs[i] != BADPID) {

            // kprintf("gethigh  %d  %d\n", proctab[i].pstate, proctab[i].pinh);
            return (proctab[i].pinh);
        }
    }
    return 0;
}

void updateholdprio (int ldes)
{
    struct lentry *lptr;
    struct pentry *pptr;
    int i, mpid, pprio;
    lptr = &ltable[ldes];
    
    if (lptr->lpprio == -1) {
        lptr->lpprio = gethighprio (ldes);
    }

    for (i = 0; i < NPROC; ++i) {
        if (lptr->lholdprocs[i] != BADPID) {
            
            proctab[i].pprio = lptr->lpprio;
            // kprintf("pprio: %d ",proctab[i].pprio);
            // kprintf(" -> %d ",proctab[i].pinh);
            pptr = &proctab[i];
            
            mpid = checkpriority(ldes, i, pptr->pprio);
            // kprintf("mpid: %d\n", mpid);
            if (mpid != -1) {
                /* priority inheritance case */
                // kprintf("->inheriting priority\n");
                inheritprio (ldes, mpid, i);
            }
        }
    }
}

/**
 * updates the priority of process
 * when lock is released
 */
void updatepriority (int ppid)
{
	int i, j, maxprio;
	struct lentry *lptr;
    struct pentry *pptr;
	maxprio = 0;
    pptr = &proctab[ppid];
	for (j = 0; j < NLOCKS; ++j) {
		if (pptr->plock[j] != LFREE) {
			maxprio = (maxprio < ltable[j].lpprio) ? ltable[j].lpprio : maxprio;
		}	
	}
	proctab[ppid].pprio = maxprio;
}

void chprio_updateprio (int ppid)
{
	struct lentry *lptr;
	int i, maxprio;
	maxprio = 0;
	lptr = &ltable[proctab[ppid].lockid];
	for (i = 0; i < NPROC; ++i) {
		if (lptr->lqwait[i] != BADPID && proctab[i].pprio > maxprio) {
			maxprio = proctab[i].pprio;
		}
	}
	for (i = 0; i < NPROC; ++i) {
		if (lptr->lqwait[i] != BADPID && proctab[i].pprio < maxprio) {
            proctab[i].pprio = maxprio;
        }
		if (proctab[i].plock[proctab[ppid].lockid] != LFREE && proctab[i].pprio < maxprio) {
			proctab[i].pprio = maxprio;
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
        lptr->lholdprocs[currpid] = BADPID;
		proctab[currpid].plock[currlock] = LFREE;
	}
	updatepriority (currpid);
    restore (ps);
    return(res);
}