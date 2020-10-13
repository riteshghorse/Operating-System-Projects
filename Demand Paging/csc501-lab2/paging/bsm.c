/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


bsm_map_t bsm_tab[NBS];
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	STATWORD ps;
	int i, j;
	disable(ps);
	for (i = 0; i < NBS; ++i) {
		bsm_tab[i].bs_access		= 0;
		bsm_tab[i].bs_status		= BSM_UNMAPPED;
		for (j = 0; j < NPROC; ++j) {
			bsm_tab[i].bs_pid[j]	= BADPID;
			bsm_tab[i].bs_vpno[j]	= 0
			bsm_tab[i].bs_npages[j]	= 0
		}
		bsm_tab[i].bs_sem = 0;
	}
		

	restore(ps);
	return(OK);
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	int i;
	disable(ps);
	
	for (i = 0; i < NBS; ++i) {
		if (bsm_tab[i].bs_status == BSM_UNMAPPED ) {
			/* backing store found */
			*avail = i;
			break;
		}
	}
	restore(ps);
	if (i >= NBS)	/* no backing store found */
		return(SYSERR);

	return(OK);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	int i;
	disable(ps);
	
	bsm_tab[i].bs_access = 0;
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	for(p = 0; p < NPROC; ++p)
		bsm_tab[i].bs_pid[p] = BADPID;
	bsm_tab[i].bs_sem = 0;	

	restore(ps);
	return(OK);
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	if (npages <= 0 || npages > 256)
		return(SYSERR);

	STATWORD ps;
	disable(ps);
	bsm_tab[source].bs_access = 0;;
	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid[pid] = pid;
	bsm_tab[source].bs_vpno[pid] = vpno;
	bsm_tab[source].bs_npages[pid] = npages;
	bsm_tab[source].sem = 0;	
	restore(ps);
	return(OK);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}


