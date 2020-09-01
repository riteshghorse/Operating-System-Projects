#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>


int isprocactive = 0;
unsigned long ctr1000;
struct sctrace sctrtable[50][27] = {"", 0, 0.0};

void 
syscallsummary_start ()
{
	isprocactive = 1;
}

void 
syscallsummary_stop ()
{
	isprocactive = 0;
}

void 
printsyscallsummary ()
{
	int i, j, flag;
	unsigned long averagetime = 0;
	const int numprocs = NPROC;
	kprintf ("\nvoid printsyscallsummary()\n");
	
	for (i = 0; i < numprocs; ++i) {
	 	flag = 0;
   		for (j = 0; j < 27; ++j) {
			if (sctrtable[i][j].frequency > 0) {
				if (flag == 0) {
					flag = 1;
					kprintf ("Process [pid:%d]\n", i);
				}
				averagetime = (sctrtable[i][j].totaltime) / (sctrtable[i][j].frequency);
				kprintf ("\tSyscall: %s, count: %d, average execution time: %lu(ms)\n",	sctrtable[i][j].name, sctrtable[i][j].frequency, averagetime);
				sctrtable[i][j].frequency = 0;
				strcpy(sctrtable[i][j].name, "");
				sctrtable[i][j].totaltime = 0.0;
			}
		}
	}

}
