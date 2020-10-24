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
	if (bsm_tab[bs_id].bs_access == 1 && bsm_tab[bs_id].bs_pid[currpid] != currpid) {
		restore (ps);
		return(SYSERR);
	}
	if (bsm_tab[bs_id].bs_access == 1 && bsm_tab[bs_id].bs_pid[currpid] == currpid)
		bsm_tab[bs_id].bs_access = 0;

	bsm_tab[bs_id].bs_pid[currpid] = BADPID;
	bsm_tab[bs_id].bs_vpno[currpid] = 0;
	bsm_tab[bs_id].bs_npages = 0;
	bsm_tab[bs_id].bs_refcnt -= 1;
	if (bsm_tab[bs_id].bs_refcnt <= 0)	
		bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	restore(ps);
	return OK;
}

