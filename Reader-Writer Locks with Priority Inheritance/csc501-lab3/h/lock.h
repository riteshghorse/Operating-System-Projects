/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCKS
#define NLOCKS  50      /* number of LOCKs, if not defined */
#endif

#define LFREE   '\01'           /* this LOCK is free               */
#define LUSED   '\02'           /* this LOCK is used               */
#define WRITE   '\01'           /* this LOCK is free               */
#define READ   '\02'           /* this LOCK is used               */

struct  lentry  {               /* LOCK table entry                */
        char    ltype;         /* the state SFREE or SUSED             */
        char    lstate;
	int 	lprio;		/* max priorty for the lock acquired */
        int     lockcnt;         /* count for this LOCK             */
        int     lqhead;         /* q index of head of list              */
        int     lqtail;         /* q index of tail of list              */
        int     lqwait[NPROC];  /* processes waiting for lock */
        int     lqtype[NPROC];  /* type of request */
};

extern  struct  lentry  ltable[];
extern  int     nextlock;

#define isbadlock(l)     (l<0 || l>=NLOCKS)


extern void linit();

#endif
