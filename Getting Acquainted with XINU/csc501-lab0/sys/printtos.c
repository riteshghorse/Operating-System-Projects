#include <stdio.h>
#include <lab0.h>

unsigned long *ebp;
unsigned long *esp;

void 
printtos ()
{
	asm ("movl %ebp,ebp");
	kprintf ("\nvoid printtos()\n");
	kprintf ("Before[0x%08x]: 0x%08x\n", ebp+2, *(ebp+2));
	kprintf ("After [0x%08x]: 0x%08x\n", ebp, *ebp);
	asm ("movl %esp,esp");
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;
	++esp;
	kprintf ("\t\telement[0x%08x]: 0x%08x\n", esp,*esp);
	++esp;
	kprintf ("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
	++esp;
	kprintf ("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
	++esp;
	kprintf ("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
} 
