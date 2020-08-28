/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <lab0.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
	unsigned long starttime, endtime;
	const int syscallid = 4;
	starttime = ctr1000;
	int currpid = getpid();
	if (isprocactive == 1) {
		strcpy (sctrtable[currpid][syscallid].name, "sys_gettime");
		sctrtable[currpid][syscallid].frequency += 1;
		endtime = ctr1000;
		sctrtable[currpid][syscallid].totaltime += endtime - starttime;
	}
   	return OK;
}
