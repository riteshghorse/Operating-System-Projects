/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
//	kprintf("in pfint\n");
	STATWORD ps;
	int rc, pdframe, ptframe;
	int store, pageth;
	unsigned long fa_addr;
	
	disable(ps);

	fa_addr = read_cr2 ();
	virt_addr_t *addr = (virt_addr_t*)&fa_addr;
	
	pd_t *pd = (pd_t*)(proctab[currpid].pdbr + (addr->pd_offset) * (sizeof(pd_t)));

	if (!ispresent(pd->pd_pres)) {
		pdframe = newpagetable(currpid);
		// init_pde (pd);
		pd->pd_pres = 1;
		pd->pd_base = FRAME0 + pdframe;
		frm_tab[pdframe].fr_refcnt += 1;
		frm_tab[pdframe].fr_type = FR_TBL;
	}	
	// kprintf("%u ", pd->pd_base);
	pt_t *pte = (pt_t*)((pd->pd_base)*NBPG + (addr->pt_offset) * (sizeof(pt_t)));
	// init_pte (pte);
	if (pte->pt_pres == 0) {
		rc = bsm_lookup (currpid, fa_addr, &store, &pageth);
		if (rc == (SYSERR)) {
				// kprintf("killed\n");
				kill (currpid);
				restore (ps);
				return(SYSERR);
		}
		rc = get_frm(&ptframe);
		if (rc == (SYSERR)) {
		//	kill (currpid);
			restore (ps);
			return(SYSERR);
		}
		pte->pt_pres = 1;
		pte->pt_write = 1;
		pte->pt_base = ptframe + FRAME0;
	} else {
		ptframe = pte->pt_base - FRAME0;
		// frm_tab[ptframe].fr_refcnt += 1;
	}
	
	pte->pt_acc = 1;
	// kprintf("getting frm\n");
	
	// init_frame_tab (ptframe);
	frm_tab[ptframe].fr_status = FRM_MAPPED;	
	frm_tab[ptframe].fr_pid = currpid;
	frm_tab[ptframe].fr_vpno = fa_addr/NBPG;
	frm_tab[ptframe].fr_type = FR_PAGE;
	frm_tab[ptframe].fr_refcnt += 1;
	
	read_bs ((char*)((ptframe + FRAME0) * NBPG), store, pageth);
	
	// kprintf(" %u\n", pt->pt_base);
	/* flush tlb */
	write_cr3 (proctab[currpid].pdbr);

	restore (ps);
	return OK;
}


