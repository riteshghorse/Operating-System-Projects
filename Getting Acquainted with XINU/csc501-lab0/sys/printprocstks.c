#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long	*esp;

void
printprocstks (int priority)
{
	kprintf("void printprocstks(int priority)\n");
	int processes = NPROC;	/* number of processes */		
	int i = 0;
	struct pentry *currprocess;

	for (i = 0; i < processes; ++i) {
		currprocess = &proctab[i];		
		if (currprocess->pstate != PRFREE) {
			if(currprocess->pprio > priority) {
					kprintf ("Process [%s]\n", currprocess->pname);
					kprintf ("\tpid: %d\n", i);
					kprintf ("\tpriority: %d\n", currprocess->pprio);
					kprintf ("\tbase: 0x%08x\n", currprocess->pbase);
					kprintf ("\tlimit: 0x%08x\n", currprocess->plimit);
					kprintf ("\tlen: %d\n", currprocess->pstklen);
					if (currprocess->pstate == PRCURR) {
						asm ("movl %esp,esp");
						kprintf ("\tpointer: 0x%08x\n", esp);
					} else {
						esp = (unsigned long*)currprocess->pesp;
						kprintf("\tpointer: 0x%08x", esp);
					}
			}
		}
	}
}
