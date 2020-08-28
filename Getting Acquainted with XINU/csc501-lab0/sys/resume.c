/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL resume(int pid)
{
	unsigned long starttime = ctr1000;	
	unsigned long endtime;
	const int syscallid = 9;

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	if (isprocactive == 1) {
		strcpy(sctrtable[pid][syscallid].name, "sys_resume");
		sctrtable[pid][syscallid].frequency += 1;
		endtime = ctr1000;
		sctrtable[pid][syscallid].totaltime += endtime - starttime;
	}
	return(prio);
}
