#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
	
	STATWORD ps;
	int rc;
	disable(ps);

	/* invalid requests */	
	if (npages <= 0 || npages > 256) {
		restore(ps);
		return(SYSERR);
	}
	
	if (bs_id < 0 || bs_id >= 8) {
                restore(ps);
                return(SYSERR);
        }
	if (bsm_tab[bs_id].bs_status == BSM_UNMAPPED) {
		restore(ps);
		return npages;
	}
        /* check if backing store is private and used by other process */
        if (bsm_tab[bs_id].bs_access == 1 || bsm_tab[bs_id].bs_pid != currpid) {
                restore(ps);
                return(SYSERR);
        }
	if (bsm_tab[bs_id].bs_pid == currpid) {
		restore(ps);
		return bsm_tab[bs_id].bs_npages[currpid];
	}
	restore(ps);
	return npages;
}


