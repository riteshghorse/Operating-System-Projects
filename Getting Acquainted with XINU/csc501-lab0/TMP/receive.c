/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL	receive()
{
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	unsigned long starttime, endtime;
	const int syscallid = 6;
	starttime = ctr1000;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);
	if (isprocactive == 1) {
		strcpy (sctrtable[currpid][syscallid].name, "sys_receive");
		sctrtable[currpid][syscallid].frequency += 1;
		endtime = ctr1000;
		sctrtable[currpid][syscallid].totaltime += endtime - starttime; 
	}
	return(msg);
}
