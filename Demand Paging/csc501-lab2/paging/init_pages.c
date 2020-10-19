#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

unsigned long reg;

SYSCALL init_global_pagetables ()
{
	int i, j, phyframe, rc;
	int frame_base;
	for (i = 0; i < 4; ++i) {
		rc = get_frm(&phyframe);	/* get a free frame     */
		if (rc == (SYSERR)){		/* free frame not found */
			return(SYSERR);
		}
		/* frame mapping for free frame obtained */
		init_frame_tab ();
		frm_tab[phyframe].fr_status = FRM_MAPPED;
		frm_tab[phyframe].fr_pid = NULLPROC;
		frame_base = (FRAME0 + phyframe) * NBPG; 
		for (j = 0; j < MAX_PTE; ++j) {
			pt_t *pte = (pt_t*)(frame_base + (j * (sizeof(pt_t))));
			init_pte (pte);
			pte->pt_pres = 1;
			pte->pt_write = 1;
			pte->pt_global = 1;
			pte->pt_base = j + (i * FRAME0);
		} 	
	}
	return(OK);		
}


SYSCALL init_page_directory (int pid)
{
	int i, phyframe, rc, pd_base;
	
	rc = get_frm (&phyframe);
	if (rc == (SYSERR)){            /* free frame not found */
		return(SYSERR);
        }

	init_frame_tab ();
	frm_tab[phyframe].fr_status = FRM_MAPPED;
        frm_tab[phyframe].fr_pid = NULLPROC;
	proctab[pid].pdbr = ((FRAME0 + phyframe) * NBPG);
	pd_base = proctab[pid].pdbr;
	for (i = 0; i < MAX_PDE ; ++i) {
		pd_t *pde = (pd_t*)(pd_base + (i * sizeof(pd_t)));
		init_pde (pde);
		if (i < 4) {
			pde->pd_pres = 1;
			pde->pd_base = FRAME0 + i;
		}
	}		
	return(OK);
}


void init_pte (pt_t *pte)
{
	pte->pt_pres = 0;
        pte->pt_write = 0;
        pte->pt_user = 0;
        pte->pt_pwt = 0;
        pte->pt_pcd = 0;
        pte->pt_acc = 0;
        pte->pt_dirty = 0;
        pte->pt_mbz = 0;        
        pte->pt_global = 0;
        pte->pt_avail = 0;
        pte->pt_base = 0;	
}

void init_pde (pd_t *pde)
{
        pde->pd_pres = 0;
        pde->pd_write = 1;
        pde->pd_user = 0;
        pde->pd_pwt = 0;
        pde->pd_pcd = 0;
        pde->pd_acc = 0;
        pde->pd_mbz = 0;
        pde->pd_fmb = 0;
	pde->pd_global = 0;
        pde->pd_avail = 0;
        pde->pd_base = 0;
}

int newpagetable (int pid)
{
	int i, pframe;
	int rc, ptframe;
	pt_t *pt;

	rc = get_frm (&pframe);
	if (rc == (SYSERR))
		return(SYSERR);

	ptframe = FRAME0 + pframe;
	init_frame_tab (pframe);
	frm_tab[pframe].fr_status = FRM_MAPPED;
	frm_tab[pframe].fr_pid = pid;
	
	for (i = 0; i < 1024; ++i) {
		pt = (pt_t*)(ptframe * NBPG + i * sizeof(pt_t));
		init_pte (pt);			
	}		
	return pframe;
}

SYSCALL setcr3 (unsigned long pdbr)
{
	reg = pdbr;
	STATWORD ps;
	disable (ps);
	asm ("pushl %eax");
	asm ("movl reg,%eax");
	asm ("movl %eax,%cr3");
	asm ("popl %eax");	
	restore (ps);
	return(OK);
}



/*
 *  *enable paging by setting 31st bit of cr0
 *   * */

SYSCALL getcr0 (unsigned long *cr0)
{
        STATWORD ps;
//        unsigned long eax;
        disable (ps);
        asm ("pushl %eax");
        asm ("movl %cr0,%eax");
        asm ("movl %eax,reg");
        asm ("popl %eax");
        *cr0 = reg;
        restore (ps);
        return(OK);
}

SYSCALL setcr0 (unsigned long cr0)
{
 	reg = cr0;
        STATWORD ps;
        disable (ps);
	asm ("pushl %eax");
        asm ("movl reg,%eax");
        asm ("movl %eax,%cr0");
        asm ("popl %eax");
        restore (ps);
        return(OK);
}

void enablepaging ()
{
	unsigned long cr0;
	getcr0 (&cr0);
	cr0 = (cr0 | ((0x1<<31) | 0x1));
	setcr0 (cr0);
}

SYSCALL getcr2 (unsigned long *cr2)
{
	STATWORD ps;
	//unsigned long eax;
	disable (ps);
	asm ("pushl %eax");
	asm ("movl %cr2,%eax");
	asm ("movl %eax,reg");
	asm ("popl %eax");
	*cr2 = reg;
	restore (ps);
	return(OK);
}
