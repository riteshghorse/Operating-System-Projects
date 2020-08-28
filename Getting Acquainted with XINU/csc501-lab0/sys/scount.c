/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL scount(int sem)
{
	extern	struct	sentry	semaph[];
	unsigned long starttime, endtime;
	const int syscallid = 10;
	int currpid = getpid();
	starttime = ctr1000;

	if (isprocactive == 1) {
                strcpy (sctrtable[currpid][syscallid].name, "sys_scount");
                sctrtable[currpid][syscallid].frequency += 1;
                endtime = ctr1000;
                sctrtable[currpid][syscallid].totaltime += endtime - starttime    ;
        } 
	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
		return(SYSERR);
	return(semaph[sem].semcnt);
}
