/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	

	/* free acquired backing stores */
	for (i = 0; i < NBS; ++i) {
		if (bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_pid[pid] == pid) {
			// if (bsm_tab[i].bs_refcnt > 0)
			// 	free_bsm (i);
/*			bsm_unmap (pid, bsm_tab[i].bs_vpno, 0);
			release_bs (i);
*/
			bsm_tab[i].bs_pid[pid] = BADPID;
			bsm_tab[i].bs_vpno[pid] = 4096;
			bsm_tab[i].bs_refcnt = -1;
			if (bsm_tab[i].bs_refcnt <= 0)
				free_bsm (i);
		}
	}

	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
