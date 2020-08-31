/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	unsigned long starttime, endtime;
	const int syscallid = 3;

	starttime = ctr1000;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	restore(ps);

	if (isprocactive == 1) {
		strcpy (sctrtable[currpid][syscallid].name, "sys_getprio");
		sctrtable[currpid][syscallid].frequency += 1;
		endtime = ctr1000;
		sctrtable[currpid][syscallid].totaltime += endtime - starttime;
	}
	return(pptr->pprio);
}
