#include <lab0.h>

extern char etext, edata, end;
void printsegaddress()
{
	kprintf("void printsegaddress()\n");
	kprintf("Current: etext[0x%x]=0x%08x, ", &etext, etext);
	kprintf("edata[0x%x]=0x%08x, ", &edata, edata);
	kprintf("ebss[0x%x]=0x%08x\n", &end, end);
	
	etext--;
	edata--;
	end--;
	kprintf("Preceding: etext[0x%x]=0x%08x, ", &etext, etext);
	kprintf("edata[0x%x]=0x%08x, ", &edata, edata);
	kprintf("ebss[0x%x]=0x%08x\n", &end, end);

	etext++; etext++;
	edata++; edata++;
	end++; end++;
	kprintf("After: etext[0x%x]=0x%08x, ", &etext, etext);
	kprintf("edata[0x%x]=0x%08x, ", &edata, edata);
	kprintf("ebss[0x%x]=0x%08x\n", &end, end);
}
