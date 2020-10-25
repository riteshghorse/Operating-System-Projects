/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
/*	kprintf("To be implemented!\n");*/
	STATWORD 	ps;
	int	 	pid;
	struct pentry 	*pptr;
	bsd_t 		bs_id;	
	int		rc;

	disable(ps);
	pid = create(procaddr, ssize, priority, name, nargs, args);
	/* error in creating a new process */
	if (pid == (SYSERR)) {
		restore (ps);
		return(SYSERR);
	}	

	rc = get_bsm (&bs_id);
	if (rc == (SYSERR)) {
		restore (ps);
		return(SYSERR);
	}
	
	rc = bsm_map (pid, 4096, bs_id, hsize);
	if (rc == (SYSERR)) {
		restore (ps);
		return(SYSERR);
	}		

	bsm_tab[bs_id].bs_access = 1;
	proctab[pid].store = bs_id;
	proctab[pid].vhpno = 4096;
	proctab[pid].vhpnpages = hsize;
	proctab[pid].vmemlist = getmem (sizeof(struct mblock *));
	proctab[pid].vmemlist->mnext = (struct mblock*)roundmb(4096 *  NBPG);
	proctab[pid].vmemlist->mlen = 0;

	struct mblock * memblock = (unsigned int)((2048 + bs_id * 256)*NBPG);
    memblock->mnext = 0;  
    memblock->mlen  = hsize*NBPG;
	restore(ps);
	return pid;
}


/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}