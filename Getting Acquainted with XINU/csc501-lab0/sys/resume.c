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
extern unsigned long ctr1000;
extern struct sctrace sctrtable[50][27];

SYSCALL resume(int pid)
{
	unsigned long starttime = ctr1000;	
	unsigned long endtime;
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
	endtime = ctr1000;
	if (isprocactive == 1) {
		strcpy(sctrtable[pid][10].name, __func__);
		sctrtable[pid][10].frequency += 1;
		sctrtable[pid][10].totaltime += endtime - starttime;
	}
	return(prio);
}
