/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL	setnok(int nok, int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	
	unsigned long starttime, endtime;
	const int syscallid = 14;
	starttime = ctr1000;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	if (isprocactive == 1) {
                strcpy (sctrtable[currpid][syscallid].name, "sys_setnok");
                sctrtable[currpid][syscallid].frequency += 1;
                endtime = ctr1000;
                sctrtable[currpid][syscallid].totaltime += endtime - starttime    ;
        }   
	return(OK);
}
