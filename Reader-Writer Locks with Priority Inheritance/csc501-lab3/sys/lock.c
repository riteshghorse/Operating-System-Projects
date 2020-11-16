
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


extern int ctr1000;
/**
 *
 * returns true if no waiting process 
 * has priority greater than current proc
 **/
int allowread(int ldes, int priority)
{
    struct lentry *ldtptr;
    // int maxprio = MININT;
    int curr;
    ldtptr = &ltable[ldes];
    
    curr = ldtptr->lqtail;
    while (q[curr].qprev != ldtptr->lqhead) {
            curr = q[curr].qprev;
            if (q[curr].qkey >= priority && ldtptr->lqtype[curr] == WRITE){
                // kprintf("read not allowed with %d:%d\n", currpid, priority);
                return 0;   /* read not allowed*/
            }
    }
    // if (priority < ldtptr->lprio) {
    //     return 0;
    // }
    return 1;   /* read allowed */
}

/**
 * returns -1 if p2->pprio > p1->pprio
 */
int checkpriority(int ldes, int ppid, int priority)
{
    int i, pprio;

    for (i = 0; i < NPROC; ++i) {
        if (i != ppid && proctab[i].pstate != PRFREE) {
            if (proctab[i].pinh != 0)
                pprio = proctab[i].pinh;
            else 
                pprio = proctab[i].pprio;
            if (proctab[i].plock[ldes] != LFREE && pprio < priority)
                return i;   /* need to update the priority */
        }
    }
    return -1;   /* all well */
}

/**
 * priority inheritance
 */ 
int inheritprio(int ldes, int p2, int p1)
{
    int npid;
    if (proctab[p2].pinh == 0)
        proctab[p2].pinh = proctab[p2].pprio;
    if (proctab[p1].pinh != 0)
        proctab[p2].pprio = proctab[p1].pinh;
    else 
        proctab[p2].pprio = proctab[p1].pprio;

    if (proctab[p2].lockid != -1) {
        npid = checkpriority (proctab[p2].lockid, p2, proctab[p2].pinh);
        if (npid != -1)
            inheritprio(proctab[p2].lockid, npid, p2);
    }
}

/**------------------------------------------------------------------------
 * lock  --  tries to acquire the lock
 * 
 *------------------------------------------------------------------------ 
 */
int lock(int ldes1, int type, int priority)
{
    // kprintf("lock %d  : %d\n", );
    STATWORD ps;
    struct pentry *pptr;
    struct lentry *lptr;
    int mpid, pprio;
    disable(ps);
    pptr = &proctab[currpid];
    lptr = &ltable[ldes1];
    // kprintf("lock %d  : %d\n", lptr->lstate, lptr->ltype);
    /* check if lock is in deleted state*/
    if (isbadlock(ldes1) || 
        lptr->lstate == LFREE) {
        kprintf("Error in locking\n");
        restore(ps);
        return(SYSERR);
    }

    /* id free allocate directly */
    if (lptr->lstate == LUSED && lptr->ltype == LFREE) {
        // kprintf("Lock was free\n");
        lptr->lockcnt = 1;
        lptr->ltype = type;
        lptr->lprio = priority;
        lptr->lholdprocs[currpid] = currpid;
        pptr->plock[ldes1] = type;
        pptr->lwaitret = (OK);
        restore(ps);
        return(OK);
    }

    /* if currently a write lock then just add to queue */
    if (lptr->ltype == WRITE) {
        if (pptr->pinh != 0)
            pprio = pptr->pinh;
        else
            pprio = pptr->pprio;
        mpid = checkpriority(ldes1, currpid, pprio);
        // kprintf("mpid: %d\n", mpid);
        if (mpid != -1) {
            /* priority inheritance case */
            // kprintf("->inheriting priority\n");
            inheritprio (ldes1, mpid, currpid);
        }
        // kprintf("Write lock exists\n");
        pptr->pstate = PRWAIT;
        // pptr->plock[ldes1] = ldes1;
        lptr->lqwait[currpid] = currpid;
        lptr->lqtype[currpid] = type;
        if (lptr->lpprio < pptr->pprio)
            lptr->lpprio = pptr->pprio;
        pptr->lqwaittime = ctr1000;
        pptr->lockid = ldes1;
        insert (currpid, lptr->lqhead, priority);
        pptr->lwaitret = DELETED;
        resched();
        restore(ps);
        return pptr->lwaitret;
    }   

    /* if currently a read lock */
    if (lptr->ltype == READ) {
        if (type == WRITE) {
            if (pptr->pinh != 0)
                pprio = pptr->pinh;
            else
                pprio = pptr->pprio;
            mpid = checkpriority(ldes1, currpid, pprio);
            if (mpid != -1) {
                /* priority inheritance case */
                // kprintf("->inheriting priority\n");
                inheritprio (ldes1, mpid, currpid);
            }
            // kprintf("Read lock exists\n");
            pptr->pstate = PRWAIT;
            // pptr->plock[ldes1] = ldes1;
            lptr->lqwait[currpid] = currpid;
            lptr->lqtype[currpid] = type;
            if (lptr->lpprio < pptr->pprio)
                lptr->lpprio = pptr->pprio;
            pptr->lqwaittime = ctr1000;
            pptr->lockid = ldes1;
            insert (currpid, lptr->lqhead, priority);
            pptr->lwaitret = DELETED;
            resched();
            restore(ps);
            return pptr->lwaitret;
        } else {
            /* check if there is another write process waiting with greater priority */
            if (allowread(ldes1, priority)) {
                // kprintf("Allowing read...\n");
                lptr->lockcnt += 1;
                lptr->lholdprocs[currpid] = currpid;
                proctab[currpid].plock[ldes1] = READ;
                proctab[currpid].lwaitret = (OK);
                // proctab[currpid].plock[ldes1] = 1;
                restore (ps);
                return(OK);
            } else {
                if (pptr->pinh != 0)
                    pprio = pptr->pinh;
                else
                    pprio = pptr->pprio;
                mpid = checkpriority(ldes1, currpid, pprio);
                if (mpid != -1) {
                    /* priority inheritance case */
                    // kprintf("->inheriting priority\n");
                    inheritprio (ldes1, mpid, currpid);
                }
                // kprintf("Can't allow read\n");
                pptr->pstate = PRWAIT;
                // pptr->plock[ldes1] = ldes1;
                lptr->lqwait[currpid] = currpid;
                lptr->lqtype[currpid] = type;
                if (lptr->lpprio < pptr->pprio)
                    lptr->lpprio = pptr->pprio;
                pptr->lqwaittime = ctr1000;
                pptr->lockid = ldes1;
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
