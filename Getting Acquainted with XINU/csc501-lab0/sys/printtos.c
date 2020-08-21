#include <stdio.h>
unsigned long *ebp;
unsigned long *esp;
void printtos()
{
	asm("movl %ebp,ebp");
	kprintf("void printtos()\n");
	kprintf("Before[0x%08x]: 0x%08x\n", (ebp+2), *(ebp+2));
	kprintf("After [0x%08x]: 0x%08x\n", ebp, *ebp);
	asm("movl %esp,esp");
	++esp;
	kprintf("\t\telement[0x%08x]: 0x%08x\n", esp,*esp);
	++esp;
	kprintf("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
	++esp;
	kprintf("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
	++esp;
	kprintf("\t\telement[0x%08x]: 0x%08x\n", esp, *esp);
} 
