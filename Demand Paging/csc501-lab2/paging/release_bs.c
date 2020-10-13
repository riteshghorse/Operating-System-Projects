#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
//    kprintf("To be implemented!\n");
	STATWORD ps;
	disable(ps);
	if (bs_id < 0 || bs_id >= NBS) {
		restore (ps);
		return(SYSERR);
	}
	bsm_tab[bs_id].bs_access = 0;
	bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_id].bs_npages = 0;
	bsm_tab[bs_id].bs_sem = 0;
	restore(ps);
	return OK;
}

