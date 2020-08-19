/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

int main()
{
	long num = 0xaabbccdd, ans;
	kprintf("\n\nHey World, Xinu lives\n\n");
	ans = zfunction(num);
	kprintf("num: 0x%x\n", num);
	kprintf("ans: 0x%x\n", ans);
	return 0;
}
