/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL signal(int sem)
{
	STATWORD ps;    
	register struct	sentry	*sptr;
	
	unsigned long starttime, endtime;
	const int syscallid = 16;
	starttime = ctr1000;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);

	if (isprocactive == 1) {
                strcpy (sctrtable[currpid][syscallid].name, "sys_signal");
                sctrtable[currpid][syscallid].frequency += 1;
                endtime = ctr1000;
                sctrtable[currpid][syscallid].totaltime += endtime - starttime    ;
        }   
	return(OK);
}
