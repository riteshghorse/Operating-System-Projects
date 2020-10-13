/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
/*  kprintf("xmmap - to be implemented!\n");*/
	
	STATWORD ps;
	disable(ps);
	
	/* if invalid backing store */
	if (source < 0 || source >= 8 || npages <= 0 || npages > 256) {
		restore(ps);
		return(SYSERR);
	}
	/* check if backing store is private and used by other process */
	if (bsm_tab[source].bs_access == 1 && bsm_tab[source].bs_pid[currpid] != currpid) {
		restore(ps);
		return(SYSERR);
	}
	/* if already mapped */
	if (bsm_tab[source].bs_status == BSM_UNMAPPED){
		bsm_tab[source].bs_access = 0;
		bsm_tab[source].bs_status = BSM_MAPPED;
	}
	bsm_tab[source].bs_pid[currpid] = currpid;
	bsm_tab[source].bs_vpno[currpid] = virtpage;
	bsm_tab[source].bs_npages[currpid] = npages;
	
	restore(ps);
	return(OK);
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	
	return SYSERR;
}
