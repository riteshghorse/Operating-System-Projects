#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>
#include <math.h>

int schedclass = 0;	/* initialize to 0 */

/* set scheduler class */
void setschedclass (int sched_class)
{
	schedclass = sched_class;
}


/* returns the scheduler class */
int getschedclass ()
{
	return schedclass;
}


int getnextexpproc (int expval)
{
	int nextprocess;
	/* check if ready queues has processes */
	if (isempty(rdyhead))
		return (NULLPROC);
	
	/* return lastkey if expval no less than lastkey */
	if (expval >= lastkey(rdytail)) {
		return (q[rdytail].qprev);
	}
	
	/* return firstkey if expval no greater than firstkey*/
	if (expval < firstkey(rdyhead)) {
		return (q[rdyhead].qnext);
	}

	nextprocess = q[rdyhead].qnext;
	while (nextprocess < NPROC && expval >= q[nextprocess].qkey) {
		nextprocess = q[nextprocess].qnext;
	}
	if (nextprocess >= NPROC)
		return(NULLPROC);
	/* check for round robin condition */
	while (nonempty(nextprocess) && (q[nextprocess].qkey == q[q[nextprocess].qnext].qkey))
		nextprocess = q[nextprocess].qnext;
	return nextprocess;
}

int isendofepoch () 
{
	int nextprocess;
	nextprocess = q[rdyhead].qnext;
	while (nextprocess != rdytail && nextprocess < NPROC) {
		if(proctab[nextprocess].goodness > 0)	/* not the end of epoch */
			return 0;	
		nextprocess = q[nextprocess].qnext;
	}
	return 1;	/* end of epoch */
}

void newepochinit ()
{
	int i;
	for (i = 0; i < NPROC; ++i) {
		if (proctab[i].pstate != PRFREE) {
			/* process never executed or exhausted its quantum in last epoch */
			if (proctab[i].counter < 1) {
				proctab[i].counter = proctab[i].pprio;
			} else {
				/* process with non-zero counter in last epoch */	
				proctab[i].counter = (proctab[i].counter / 2) + proctab[i].pprio;
				//kprintf("c: %d - %d\n", proctab[i].counter, i);
			}
		}
		/* set the goodness value */
		proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
	}
}

int getnextlinuxproc ()
{
	int nextprocess, process = NPROC+1, gval = 0, temp;
	nextprocess = q[rdyhead].qnext;
	while (nextprocess != rdytail && nextprocess < NPROC) {
		/* processes with positive goodness can be scheduled */
		if (proctab[nextprocess].goodness > 0)  {
			temp = proctab[nextprocess].goodness;
			if (temp >= gval) {
				gval = temp;
				process = nextprocess;
			}
		}
		nextprocess = q[nextprocess].qnext;
	}
	if (process < NPROC)
		return process;
	else
		return(NULLPROC);
}
