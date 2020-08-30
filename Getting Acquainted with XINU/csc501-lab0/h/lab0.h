#ifndef _lab0_h_
#define _lab0_h_

#include <stdio.h>

/* task 1 */
extern long zfunction(long);


/* task 2 */
extern char etext, edata;
extern void printsegaddress();


/* task 3*/
extern void printtos();

/* task 4 */
extern void printprocstks(int priority);


/* task 5 */
extern void syscallsummary_start();
extern void syscallsummary_stop();
extern void printsyscallsummary();


extern int isprocactive;	/* 1 if tracing is on else 0*/
extern unsigned long ctr1000;	/* global time variable */

extern struct sctrace {		/* structure to maintain the syscall trace */
	char name[25];		/* name of syscall */
	long frequency;		/* number of times a syscall is called */
	unsigned long totaltime;/* total time taken for execution */
};

#endif
