#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

struct lentry ltable[NLOCKS];
int nextlock;

void linit()
{
    int i, j;
    struct lentry *lptr;
    nextlock = NLOCKS - 1;

    for (i = 0; i < NLOCKS; ++i) {
        lptr = &ltable[i];
        lptr->lstate = LFREE;
        lptr->ltype = DELETED;
        lptr->lprio = 0;
        lptr->lpprio = 0;
        lptr->lockcnt = 0;
        lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        for (j = 0; j < NPROC; ++j) {
            lptr->lholdprocs[j] = BADPID;
            lptr->lqwait[j] = BADPID;
            lptr->lqtype[j] = DELETED;
        }
    }
}
