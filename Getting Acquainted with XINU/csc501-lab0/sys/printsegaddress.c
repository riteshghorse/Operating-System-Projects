#include <lab0.h>

extern char etext, edata, end;

void 
printsegaddress ()
{
	kprintf ("\nvoid printsegaddress()\n\n");
	kprintf ("Current: etext[0x%08x]=0x%08x, ", &etext, etext);
	kprintf ("edata[0x%08x]=0x%08x, ", &edata, edata);
	kprintf ("ebss[0x%08x]=0x%08x\n", &end, end);
	
	kprintf ("Preceding: etext[0x%08x]=0x%08x, ", (&etext-1), *(&etext-1));
	kprintf ("edata[0x%08x]=0x%08x, ", (&edata-1), *(&edata-1));
	kprintf ("ebss[0x%08x]=0x%08x\n", (&end-1), *(&end-1));

	kprintf ("After: etext[0x%08x]=0x%08x, ", (&etext+1), *(&etext+1));
	kprintf ("edata[0x%08x]=0x%08x, ", (&edata+1), *(&edata+1));
	kprintf ("ebss[0x%08x]=0x%08x\n", (&end+1), *(&end+1));
}
