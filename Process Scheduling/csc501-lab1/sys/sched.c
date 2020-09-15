#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>
#include <math.h>

int schedclass;

void setschedclass (int sched_class)
{
	schedclass = sched_class;
}

int getschedclass ()
{
	return schedclass;
}

int getnextexpproc (int expval)
{
	int nextprocess;
	if (expval >= lastkey(rdytail)) {
		return q[rdytail].qprev;
	}

	if (expval < firstkey(rdyhead)) {
		return q[rdyhead].qnext;
	}
	nextprocess = q[rdyhead].qnext;
	while ((expval >= q[nextprocess].qkey) && nonempty(nextprocess)) {
		nextprocess = q[nextprocess].qnext;
	}
	if (expval < q[nextprocess].qkey) {
		while ((q[nextprocess].qkey == q[q[nextprocess].qnext].qkey))
			nextprocess = q[nextprocess].qnext;
		return nextprocess;
	} else {
		return(NULLPROC) ;
	}
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
		if(proctab[nextprocess].counter > 0)
			return 0;	
		nextprocess = q[nextprocess].qnext;
	}
	return 1;
}

void newepochinit ()
{
	int i;
	for (i = 0; i < NPROC; ++i) {
		proctab[i].goodness = proctab[i].pprio;
		if (proctab[i].pstate != PRFREE) {
			/* process never executed in previous epoch */
			if (proctab[i].quantum == -1) {
				proctab[i].quantum = proctab[i].pprio;
				proctab[i].counter = proctab[i].pprio;
			} //else if (proctab[i].counter > 0) { 
			else {
				/* process executed in last epoch */	
				proctab[i].quantum = (proctab[i].counter / 2) + proctab[i].goodness;
				proctab[i].counter = proctab[i].quantum; 
			}
		}
	}
}

int getnextlinuxproc ()
{
	int nextprocess, process = NPROC+1, gval = 0, temp;
	nextprocess = q[rdyhead].qnext;
	while (nextprocess != rdytail && nextprocess < NPROC) {
		if (proctab[nextprocess].counter > 0)  {
			temp = proctab[nextprocess].counter + proctab[nextprocess].goodness;
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
