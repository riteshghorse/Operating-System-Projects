/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>
#include <math.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int expval, newprocess, sched_class, currprio;
	double lambda = 0.1;

	sched_class = getschedclass();

	if (sched_class == EXPDISTSCHED) {	/* EXPONENTIAL SCHEDULER */
		optr = &proctab[currpid];
		expval = (int) expdev(lambda);	/* get random number from exp distribution */
		
		/* force context switch */
        	if (optr->pstate == PRCURR) {
	                optr->pstate = PRREADY;
                	insert(currpid,rdyhead,optr->pprio);
        	}
		
		newprocess = getnextexpproc(expval);

		/* remove the next highest priority process from remaining of ready list */
		if(newprocess < NPROC)
			currpid = dequeue(newprocess);
		else
			currpid = (EMPTY);
		nptr = &proctab[currpid];	
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */
		return OK;
	} else if (sched_class == LINUXSCHED) { /* LINUX SCHEDULER */
	
		optr = &proctab[currpid];
		/* extract priority */
		currprio = optr->goodness - (optr->counter);
		/* update the counter */
//                optr = &proctab[currpid];
		optr->counter = preempt;

                if (optr->counter <= 0){
			optr->counter = 0;
			optr->goodness = 0;
		} else {
			optr->goodness = (optr->counter) + currprio;
		}
		/* push current state to ready queue*/
		if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid, rdyhead, optr->pprio);
                }
	
		if(isendofepoch()) {
			newepochinit();
		}

		
		newprocess = getnextlinuxproc();

		
		if (newprocess == (NULLPROC)) {
			nptr = &proctab[ (currpid = dequeue(newprocess)) ];
			nptr->pstate = PRCURR;
			preempt = QUANTUM;
			//preempt = nptr->counter;
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                	return(OK);
		} else {
			nptr = &proctab[ (currpid = dequeue(newprocess)) ];
			nptr->pstate = PRCURR;
			preempt = nptr->counter;
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return(OK);
		}
/*		nptr = &proctab[ (currpid = dequeue(newprocess)) ];
                nptr->pstate = PRCURR;
                preempt = nptr->counter;
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                return(OK);*/
	} else {

		/* no switch needed if current process priority higher than next*/
	
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		   (lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
	
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */
		return OK;
	}
}
