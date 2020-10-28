/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	STATWORD ps;
	struct mblock *p, *q;
	struct pentry *pptr;
	unsigned front;
	
	disable (ps);
	pptr = &proctab[currpid];
	if (size == 0 || size > (256 * NBPG)) {
		restore (ps);
		return(SYSERR);
	}
	if ((unsigned)block < (unsigned)(4096 * NBPG)) {
		restore (ps);
		return(SYSERR);
	}
	if ((unsigned)block > (unsigned)(pptr->vhpno + pptr->vhpnpages)*NBPG) {
		restore (ps);
		return(SYSERR);
	}
	size = (unsigned int)roundmb(size);
	for (p = (pptr->vmemlist->mnext), q = (pptr->vmemlist);
		p != (struct mblock *)NULL && p<block; q = p, p = p->mnext);
	front=q->mlen+(unsigned)q;
	if ((front >(unsigned)block && q != (pptr->vmemlist)) ||
            (p != NULL && (size+(unsigned)block) > (unsigned)p)) {
                restore(ps);
                return(SYSERR);
	}
	if (q != (pptr->vmemlist) && front == (unsigned)block) {
		q->mlen += size;
	} else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ((unsigned)( q->mlen + (unsigned)q) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore (ps);
	return(OK);
}