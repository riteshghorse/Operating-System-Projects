/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */

extern int isprocactive;
extern struct sctrace sctrtable[50][27];
extern unsigned long ctr1000;

SYSCALL getpid()
{	
	unsigned long starttime, endtime;
	const int syscallid = 2;

	starttime = ctr1000;
	int i = 0;
	if (isprocactive == 1) {
		strcpy (sctrtable[currpid][syscallid].name, "sys_getpid");
		sctrtable[currpid][syscallid].frequency += 1;
		endtime = ctr1000;
		sctrtable[currpid][syscallid].totaltime += endtime - starttime;
	}	
	return(currpid);
}
