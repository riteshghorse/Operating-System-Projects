#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

extern int isprocactive = 0;
extern unsigned long ctr1000;
extern struct sctrace sctrtable[50][27] = {"", 0, 0.0};

void syscallsummary_start ()
{
	isprocactive = 1;
}

void syscallsummary_stop ()
{
	isprocactive = 0;
}

void printsyscallsummary ()
{
	int i, j;
	unsigned long averagetime = 0;
	kprintf ("\nvoid printsyscallsummary()\n");
	for (i = 0; i < NPROC; ++i) {
		for (j = 0; j < 27; ++j) {
			if (sctrtable[i][j].frequency > 0) {
				kprintf ("Process [pid:%d]\n", i);
				averagetime = (sctrtable[i][j].totaltime) / (sctrtable[i][j].frequency);
//				kprintf("time: %lu\n", averagetime);
				kprintf ("\tSyscall: %s, count: %d, average execution time: %lu(ms)\n",	sctrtable[i][j].name, sctrtable[i][j].frequency, averagetime);
			}
		}
	}

}
