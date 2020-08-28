#ifndef _lab0_h_
#define _lab0_h_

#include <stdio.h>

extern long zfunction(long);

extern char etext, edata;

extern void printsegaddress();

extern void printtos();

extern void syscallsummary_start();
extern void syscallsummary_stop();
extern void printsyscallsummary();

extern int isprocactive;
//extern unsigned long ctr1000;
extern struct sctrace {
	char name[25];
	long frequency;
	unsigned long totaltime;
};


//extern struct sctrace sctrtable[50][27] = {"", 0, 0.0};

#endif
