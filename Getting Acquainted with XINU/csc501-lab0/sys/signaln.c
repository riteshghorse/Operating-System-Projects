/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL signaln(int sem, int count)
{
	STATWORD ps;    
	struct	sentry	*sptr;
	
	unsigned long starttime, endtime;
	const int syscallid = 17;
	starttime = ctr1000;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	if (isprocactive == 1) {
                strcpy (sctrtable[currpid][syscallid].name, "sys_signaln");
                sctrtable[currpid][syscallid].frequency += 1;
                endtime = ctr1000;
                sctrtable[currpid][syscallid].totaltime += endtime - starttime    ;
        }   
	return(OK);
}
