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
	unsigned long starttime = ctr1000, endtime;
	int i = 0;
	if (isprocactive == 1) {
		strcpy(sctrtable[currpid][0].name, "sys_getpid");
		sctrtable[currpid][0].frequency += 1;
		endtime = ctr1000;
		sctrtable[currpid][0].totaltime += endtime - starttime;
	}	
	return(currpid);
}
