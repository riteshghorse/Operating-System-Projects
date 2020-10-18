/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	STATWORD ps;
	int rc, pdframe, ptframe;
	unsigned long fa_addr;
	virt_addr_t addr;
	disable (ps);
	fa_addr = getcr2 (&fa_addr);
	addr = (virt_addr_t)fa_addr;
	
	pd_t *pd = (pd_t*)(proctab[currpid].pdbr + addr.pd_offset * (sizeof(pd_t)));

	if (pd->pd_pres == 0) {
                pdframe = newpagetable(currpid);
		init_pde (pd)
		pd->pd_pres = 1;
		pd->pd_write = 1;
		pd->pd_base = FRAME0 + pdframe;
        }	
	
	pt_t *pt = (pt_t*)((pd->pd_base)*NBPG + addr.pt_offset * (sizeof(pt_t)));
	init_pte (pt);
	if (pt->pt_pres == 0) {
		rc = bsm_lookup (currpid, fa_addr, &store, &pageth);
        	if (rc == (SYSERR)) {
	                kill (currpid);
                	restore (ps);
        	        return(SYSERR);
  	      	}
	}
	rc = get_frm(&ptframe);
	if (rc == (SYSERR)) {
	//	kill (currpid);
		restore (ps);
		return(SYSERR);
	}
	init_frame_tab (ptframe);
	frm_tab[ptframe].fr_status = FRM_MAPPED;	
	frm_tab[ptframe].fr_pid = currpid;
	frm_tab[ptframe].fr_vpno = fa_addr/NBPG;
	frm_tab[ptframe].fr_refcnt = 1;
	frm_tab[ptframe].fr_type = FR_PAGE;
	frm_tab[pd->pd_base-FRAME0].fr_refcnt += 1;
	
	read_bs ((ptframe + FRAME0) * NBPG, store, pageth);
	pt->pt_pres = 1;
	pt->pt_write = 1;
	pt->pt_acc = 1;
	pt->pt_base = FRAME0 + ptframe;
	restore (ps);
	return OK;
}


