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
	if (source < 0 || source >= NBS || npages <= 0 || npages > 256) {
		restore(ps);
		return(SYSERR);
	}

	/* check if backing store is private and used by other process */
	if (bsm_tab[source].bs_access == 1) {
		restore(ps);
		return(SYSERR);
	}
	if (bsm_tab[source].bs_status == BSM_MAPPED && npages > bsm_tab[source].bs_npages) {
		restore(ps);
		return(SYSERR);
	}
	if (bsm_tab[source].bs_pid[currpid] == currpid) {
		restore(ps);
		return(SYSERR);
	}
	if (bsm_tab[source].bs_status == BSM_UNMAPPED){
		bsm_tab[source].bs_status = BSM_MAPPED;
		bsm_tab[source].bs_npages = npages;
	}
	bsm_tab[source].bs_pid[currpid] = currpid;
	bsm_tab[source].bs_vpno[currpid] = virtpage;
	bsm_tab[source].bs_refcnt += 1;
	
	restore(ps);
	return(OK);
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	STATWORD ps;
	int rc;
	disable (ps);
	rc = bsm_unmap (currpid, virtpage, 0);
	if (rc == (SYSERR)) {
		restore (ps);
		return(SYSERR);
	}
	restore (ps);
	return SYSERR;
}
