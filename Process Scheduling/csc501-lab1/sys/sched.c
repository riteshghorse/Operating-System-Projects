#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>
#include <math.h>

int schedclass = 0;	/* initialize to 0 */

void setschedclass (int sched_class)
{
	schedclass = sched_class;
}

int getschedclass ()
{
	return schedclass;
}
/*
int getnextexpproc (int randomValue)
{
	int process, prev_process;

	process = q[rdytail].qprev;
	prev_process = q[process].qprev;

	while((randomValue < q[prev_process].qkey) && (prev_process < NPROC) ) {
		if(q[process].qkey != q[prev_process].qkey){
			process = prev_process;
		}
		prev_process = q[prev_process].qprev;
	}
	if(process >= NPROC)
		return (NULLPROC);
	else
		return (process);
}
*/
int getnextexpproc (int expval)
{
	int nextprocess;
	//check if ready queues has processes
	if (isempty(rdyhead))
		return (NULLPROC);
	
	if (expval >= lastkey(rdytail)) {
		return (q[rdytail].qprev);
	}

	if (expval < firstkey(rdyhead)) {
		return (q[rdyhead].qnext);
	}

	nextprocess = q[rdyhead].qnext;
	while (nextprocess < NPROC && expval >= q[nextprocess].qkey) {
		nextprocess = q[nextprocess].qnext;
	}
	if (nextprocess >= NPROC)
		return(NULLPROC);
	while (nonempty(nextprocess) && (q[nextprocess].qkey == q[q[nextprocess].qnext].qkey))
		nextprocess = q[nextprocess].qnext;
	return nextprocess;
}

int getlastexpproc (int process)
{
	if (process < NPROC)
		return( dequeue(process) );
	else
		return(EMPTY);
}

int isendofepoch () 
{
	int nextprocess;
	nextprocess = q[rdyhead].qnext;
	while (nextprocess != rdytail && nextprocess < NPROC) {
		if(proctab[nextprocess].goodness > 0)
			return 0;	
		nextprocess = q[nextprocess].qnext;
	}
	return 1;
}

void newepochinit ()
{
	int i;
	for (i = 0; i < NPROC; ++i) {
		if (proctab[i].pstate != PRFREE) {
			/* process never executed */
			if (proctab[i].counter < 1) {
				proctab[i].counter = proctab[i].pprio;
			} else {
				/* process executed in last epoch */	
				proctab[i].counter = (proctab[i].counter / 2) + proctab[i].pprio;
				//kprintf("c: %d - %d\n", proctab[i].counter, i);
			}
		}
		proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
	}
}

int getnextlinuxproc ()
{
	int nextprocess, process = NPROC+1, gval = 0, temp;
	nextprocess = q[rdyhead].qnext;
	while (nextprocess != rdytail && nextprocess < NPROC) {
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
