/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */



int main()
{
	long num = 0xaabbccdd, ans;
	kprintf("\n\nHey World, Xinu lives\n\n");
	ans = zfunction(num);
	kprintf("zfunction (0x%x): 0x%x\n", num, ans);
	printsegaddress();
	printtos();	
	printprocstks(1);
	syscallsummary_start ();
	getpid();
	getpid();
	sleep(1);
	syscallsummary_stop ();
	printsyscallsummary();
	syscallsummary_start ();
	getpid();
	getpid();
	printsyscallsummary ();
	syscallsummary_stop ();
	return 0;
}
