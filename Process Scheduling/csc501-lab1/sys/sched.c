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
	int process;
	if (expval > lastkey(rdytail)) {
		return q[rdytail].qprev;
	}

	if (expval < firstkey(rdyhead)) {
		return q[rdyhead].qnext;
	}
	process = q[rdyhead].qnext;
	while (expval >= q[process].qkey && nonempty(process)) {
		process = q[process].qnext;
	}
	if (nonempty(process)) {
		while (nonempty(process) && (q[process].qkey == q[q[process].qnext].qkey))
			process = q[process].qnext;
		return process;
	} else {
		return (NULLPROC);
	}
}

int getlastexpproc (int process)
{
	if (process < NPROC)
		return( dequeue(process) );
	else
		return(EMPTY);
}
