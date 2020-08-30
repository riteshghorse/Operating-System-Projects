/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
//#include <lab0.h>
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
 	kprintf("pid: %d", getpid());	
	getpid();
	getpid();
	sleep(1);
	syscallsummary_stop ();
	printsyscallsummary();
	kprintf("\nstarting again...\n");
	syscallsummary_start ();
	getpid();
	getpid();
	printsyscallsummary ();
	syscallsummary_stop ();


	return 0;
}

/*
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
void halt();

unsigned int *esp;
void procstks(int priority){
        struct  pentry *p;
        int i;
        for(i = 0; i < NPROC; i++){
                p = &proctab[i];
                if(i >= 0 && i < NPROC && p->pprio < priority && p->pstate != PRFREE){
                        if(p->pstate == PRCURR){
                                asm("movl %esp, esp");
                                kprintf("\n***** Currently Executing Process *****\n");
                                kprintf("Current stack pointer:%t 0x%x\n", esp);
                        }
                        kprintf("Saved stack pointer:\t 0x%x\n", p->pesp);
                        kprintf("Stack base:\t 0x%x\n", p->pbase);
                        kprintf("Stack size:\t %d\n", p->pstklen);
                        kprintf("Stack limit:\t 0x%x\n", p->plimit);
                        kprintf("Process name:\t %s\n", p->pname);
                        kprintf("Process id:\t %d\n", i);
                        kprintf("Process priority:\t %d\n", p->pprio);
                        kprintf("-----------------------------------\n\n");
                }
        }
}



*------------------------------------------------------------------------
 *  *  main  --  user main program
 *   *------------------------------------------------------------------------
 *    
 *    */
/*
int main(){

	kprintf("\n\nHello World, Xinu lives\n\n");

 	resume(create(procstks, 2000, 5, "proc A", 1, 50));
	resume(create(procstks, 2000, 15, "proc B", 1, 50));
	resume(create(procstks, 2000, 30, "proc C", 1, 50));
	getpid();
	getpid();
	printsyscallsummary();
 	getpid();
	printsyscallsummary();
	syscallsummary_stop();
	getpid();
	getpid();
	getpid();
	printsyscallsummary();
	syscallsummary_start();
	getpid();
	getpid();
	printsyscallsummary();
 																

	kprintf("Zfunction(0xaabbccdd) = 0x%lX\n",zfunction(0xaabbccdd));
	printsegaddress();
	printtos();
	printprocstks(1);
//	printsyscalls();
	return 0;
}*/
