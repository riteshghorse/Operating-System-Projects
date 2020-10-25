/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


bs_map_t bsm_tab[NBS];
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
		bsm_tab[i].bs_access = 0;
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		for (j = 0; j < NPROC; ++j) {
			bsm_tab[i].bs_pid[j] = BADPID;
			bsm_tab[i].bs_vpno[j] = 4096;
			bsm_tab[i].bs_npages = 0;
		}
		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].bs_refcnt = 0;
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
		if (bsm_tab[i].bs_status == BSM_UNMAPPED) {
			/* backing store found */
			*avail = i;
			break;
		}
	}
	restore(ps);
	if (i >= NBS) {	/* no backing store found */
		restore (ps);
		return(SYSERR);
	}
	restore (ps);
	return(OK);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	int p;
	disable(ps);
	if (i < 0 || i >= NBS) {
		restore (ps);
		return(OK);
	}
	bsm_tab[i].bs_access = 0;
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	for (p = 0; p < NPROC; ++p) {
		bsm_tab[i].bs_pid[p] = BADPID;
		bsm_tab[i].bs_vpno[p] = 4096;
	}
	bsm_tab[i].bs_npages = 0;
	bsm_tab[i].bs_refcnt = 0;
	bsm_tab[i].bs_sem = 0;	

	restore(ps);
	return(OK);
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, unsigned long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	int i;
	// printf("vpno: %d\n", (unsigned long) vaddr);
	long vpno;;
	disable (ps);
	vpno = vaddr / NBPG;
	// if (vpno < 1)
	// 	vpno *= (-1);
	for (i = 0; i < NBS; ++i) {
		if (bsm_tab[i].bs_status == BSM_MAPPED) {
			if (bsm_tab[i].bs_pid[pid] == pid ) {
				// printf("vpno: %u\n", (unsigned long) vaddr);
				// kprintf("%d : %d\n",bsm_tab[i].bs_vpno[pid], vpno);
				if (bsm_tab[i].bs_vpno[pid] <= vpno) {
				
				*store = i;
				*pageth = vpno - bsm_tab[i].bs_vpno[pid];
				restore (ps);
				return(OK); 
				}
			}
		}
	}
	restore (ps);
	return(SYSERR);
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
	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid[pid] = pid;
	bsm_tab[source].bs_vpno[pid] = vpno;
	bsm_tab[source].bs_npages = npages;
	bsm_tab[source].bs_sem = 0;
	bsm_tab[source].bs_refcnt += 1;	
	restore(ps);
	return(OK);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{

	if (isbadpid(pid))
		return(SYSERR);
	if (vpno < 4096) 
		return(SYSERR);

	STATWORD ps;
	int rc, store, pageth;
	int j;
	int tstore, tpage;
	long vaddr;
	disable(ps);
	vaddr = vpno * NBPG;
	rc = bsm_lookup (pid, vaddr, &store, &pageth);
	if (rc == (SYSERR)) {
		restore (ps);
		return(SYSERR);
	}
	for (j = 0; j < NFRAMES; ++j) {
		if(frm_tab[j].fr_pid==pid ) {
			if(frm_tab[j].fr_type==FR_PAGE && bsm_lookup(pid,frm_tab[j].fr_vpno*NBPG,&tstore,&tpage)==OK){
				// kprintf("writing\n");
				if(tstore == store)
					write_bs((j+FRAME0)*NBPG,store,tpage); 
			}
		}
	}
	// write_bs((j+FRAME0)*NBPG,store,page);
	bsm_tab[store].bs_pid[pid] = BADPID;
	bsm_tab[store].bs_vpno[pid] = 4096;
	bsm_tab[store].bs_refcnt -= 1;
	if (bsm_tab[store].bs_refcnt <= 0)	
		free_bsm (store);
	restore(ps);
	return(OK);
}


