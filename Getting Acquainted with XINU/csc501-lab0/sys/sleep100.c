/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL sleep100(int n)
{
	STATWORD ps;    

	unsigned long starttime, endtime;
	const int syscallid = 20;
	starttime = ctr1000;

	if (n < 0  || clkruns==0)
	         return(SYSERR);
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if (isprocactive == 1) {
                strcpy (sctrtable[currpid][syscallid].name, "sys_sleep100");
                sctrtable[currpid][syscallid].frequency += 1;
                endtime = ctr1000;
                sctrtable[currpid][syscallid].totaltime += endtime - starttime    ;
        }   
	return(OK);
}
