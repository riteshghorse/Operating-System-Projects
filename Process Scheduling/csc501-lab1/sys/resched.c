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
	int expval, newprocess, sched_class;
	double lambda = 0.1;

	sched_class = getschedclass();

	if (sched_class == EXPDISTSCHED) {	/* EXPONENTIAL SCHEDULER */
		expval = (int) expdev(lambda);
		newprocess = getnextexpproc(expval);
		

		/*if no process in rdy queue 
		then keep executing current process*/
		/*if (newprocess == (NULLPROC)) 
			return (OK);*/
		if ((optr= &proctab[currpid])->pstate == PRCURR) {
			if (newprocess == NULLPROC)
				return (OK);
		}
	
		/* force context switch */

	        if (optr->pstate == PRCURR) {
        	        optr->pstate = PRREADY;
                	insert(currpid,rdyhead,optr->pprio);
        	}
		nptr = &proctab[ (currpid = dequeue(newprocess)) ];
		nptr->pstate = PRCURR;
		#ifdef RTCLOCK
			preempt = QUANTUM;
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return(OK);				
		
	} else if (sched_class == LINUXSCHED) { /* LINUX SCHEDULER */

		/* update the counter */
                optr = &proctab[currpid];
		optr->counter = preempt;

		if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid, rdyhead, optr->pprio);
                }

                if (optr->counter <= 0)
			optr->counter = 0;
		
		if(isendofepoch()) {
		
			newepochinit();
		//	kprintf("\n");
		}
		//kprintf("c: %d ", currpid);
		newprocess = getnextlinuxproc();
		if (newprocess == (NULLPROC)) {
			if (currpid == (NULLPROC) || isempty(rdyhead)) {
				preempt = QUANTUM;
				return(OK);
			}
			nptr = &proctab[ (currpid = dequeue(newprocess)) ];
			nptr->pstate = PRCURR;
			#ifdef RTCLOCK
				preempt = QUANTUM;
			#endif
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                	return(OK);
		} 
		nptr = &proctab[ (currpid = dequeue(newprocess)) ];
		nptr->pstate = PRCURR;
		preempt = nptr->counter;
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return(OK);
	}

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
