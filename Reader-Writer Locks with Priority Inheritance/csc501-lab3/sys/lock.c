
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


extern int ctr1000;
/**
* returns true if no waiting process 
* has priority greater than current proc
*/
int allowread(int ldes, int priority)
{
    struct lentry *ldtptr;
    // int maxprio = MININT;
    int curr;
    ldtptr = &ltable[ldes];
    
    curr = ldtptr->lqtail;
    while (q[curr].qprev != ldtptr->lqhead) {
            curr = q[curr].qprev;
            if (q[curr].qkey > priority && ldtptr->lqtype[curr] == WRITE){
                // kprintf("read not allowed with %d:%d\n", currpid, priority);
                return 0;   /* read not allowed*/
            }
    }
    if (priority <= ldtptr->lprio) {
        return 0;
    }
    return 1;   /* read allowed */
}

int lock(int ldes1, int type, int priority)
{
    STATWORD ps;
    struct pentry *pptr;
    struct lentry *lptr;

    disable(ps);
    pptr = &proctab[currpid];
    lptr = &ltable[ldes1];
    /* check if lock is in deleted state*/
    if (isbadlock(ldes1) || 
        lptr->ltype == DELETED || 
        lptr->lstate == LFREE) {
        kprintf("Error in locking\n");
        restore(ps);
        return(SYSERR);
    }

    /* id free allocate directly */
    if (lptr->lstate == LUSED && lptr->ltype == LFREE) {
        kprintf("Lock was free\n");
        lptr->lockcnt = 1;
        lptr->ltype = type;
        lptr->lprio = priority;
        pptr->plock[ldes1] = type;
        pptr->lwaitret = (OK);
        restore(ps);
        return(OK);
    }

    /* if currently a write lock then just add to queue */
    if (lptr->ltype == WRITE) {
        kprintf("Write lock exists\n");
        pptr->pstate = PRWAIT;
        // pptr->plock[ldes1] = ldes1;
        lptr->lqwait[currpid] = currpid;
        lptr->lqtype[currpid] = type;
        pptr->lqwaittime = ctr1000;
        insert (currpid, lptr->lqhead, priority);
        pptr->lwaitret = DELETED;
        resched();
        restore(ps);
        return pptr->lwaitret;
    }   

    /* if currently a read lock */
    if (lptr->ltype == READ) {
        if (type == WRITE) {
            kprintf("Read lock exists\n");
            pptr->pstate = PRWAIT;
            // pptr->plock[ldes1] = ldes1;
            lptr->lqwait[currpid] = currpid;
            lptr->lqtype[currpid] = type;
            pptr->lqwaittime = ctr1000;
            insert (currpid, lptr->lqhead, priority);
            pptr->lwaitret = DELETED;
            resched();
            restore(ps);
            return pptr->lwaitret;
        } else {
            /* check if there is another write process waiting with greater priority */
            if (allowread(ldes1, priority)) {
                kprintf("Allowing read...\n");
                lptr->lockcnt += 1;
                proctab[currpid].plock[ldes1] = READ;
                proctab[currpid].lwaitret = (OK);
                // proctab[currpid].plock[ldes1] = 1;
                restore (ps);
                return(OK);
            } else {
                kprintf("Can't allow read\n");
                pptr->pstate = PRWAIT;
                // pptr->plock[ldes1] = ldes1;
                lptr->lqwait[currpid] = currpid;
                lptr->lqtype[currpid] = type;
                pptr->lqwaittime = ctr1000;
                insert (currpid, lptr->lqhead, priority);
                pptr->lwaitret = DELETED;
                resched();
                restore(ps);
                return pptr->lwaitret;
            }
        }
    } 
    kprintf("Error encountered\n");
    restore(ps);
    return(OK);
}