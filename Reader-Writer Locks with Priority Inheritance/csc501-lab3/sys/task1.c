
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

extern int ctr1000;

int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}


void reader3 (char *msg, int lck)
{
    int     ret;

    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, READ, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock, sleep 1s\n", msg);
    sleep(2);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void reader32 (char *msg, int lck1, int lck2)
{
    int     ret;

    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck2, READ, DEFAULT_LOCK_PRIO);
    lock (lck1, READ, DEFAULT_LOCK_PRIO);
    
    kprintf ("  %s: acquired lock, sleep 1s\n", msg);
    sleep(2);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck1, lck2);
}

void writer3 (char *msg, int lck)
{
    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock, sleep 2s\n", msg);
    sleep (8);
    kprintf ("  %s: to release lock\n", msg);
    releaseall (1, lck);
}

void locks ()
{
    int     lck1, lck2;
    int     rd1, rd2;
    int     wr1;
    int start, end;
    kprintf("\nTest 3: test the lock implementation\n");
    lck1  = lcreate ();
    lck2  = lcreate ();
    // assert (lck != SYSERR, "Test 3 failed");

    rd1 = create(reader32, 2000, 45, "reader3", 2, "reader A", lck1, lck2);
    rd2 = create(reader3, 2000, 50, "reader3", 2, "reader B", lck2);
    wr1 = create(writer3, 2000, 15, "writer3", 2, "writer", lck1);
    start = ctr1000;
    kprintf("-start writer, then sleep 1s. lock granted to write (prio 15)\n");
    resume(wr1);
    sleep (1);

    kprintf("-start reader A, then sleep 1s. reader A(prio 45) blocked on the lock\n");
    resume(rd1);
    sleep (1);
    
    kprintf("-start reader B, then sleep 1s. reader B(prio 50) blocked on the lock\n");
    resume (rd2);
	sleep (4);   
    end = ctr1000;
    kprintf("Total time: %d\n", end-start);
    
	kprintf("-kill reader B, then sleep 1s\n");
	// kill (rd2);
	sleep (1);

	kprintf("-kill reader A, then sleep 1s\n");
	// kill (rd1);

    sleep (5);
}

void sreader3 (char *msg, int lck)
{
    int     ret;

    kprintf ("  %s: to acquire semaphore\n", msg);
    wait (lck);
    kprintf ("  %s: acquired semaphore, sleep 1s\n", msg);
    sleep(2);
    kprintf ("  %s: to release semaphore\n", msg);
    signal(lck);
}

void sreader32 (char *msg, int lck1, int lck2)
{
    int     ret;

    kprintf ("  %s: to acquire semaphore\n", msg);
    wait (lck2);
    wait (lck1);
    
    kprintf ("  %s: acquired semaphore, sleep 1s\n", msg);
    sleep(2);
    kprintf ("  %s: to release semaphore\n", msg);
    
    signal(lck2);
    signal(lck1);
    
}

void swriter3 (char *msg, int lck)
{
    kprintf ("  %s: to acquire semaphore\n", msg);
    wait (lck);
    kprintf ("  %s: acquired semaphore, sleep 2s\n", msg);
    sleep (8);
    kprintf ("  %s: to release semaphore\n", msg);
    signal(lck);
}

void semaphores()
{
    int     lck1, lck2;
    int     rd1, rd2;
    int     wr1;
    int start, end;

    kprintf("\nTest 3: test the semaphore implementation\n");
    lck1  = screate (1);
    lck2  = screate (1);
    // assert (lck != SYSERR, "Test 3 failed");

    rd1 = create(sreader32, 2000, 45, "reader3", 2, "reader A", lck1, lck2);
    rd2 = create(sreader3, 2000, 50, "reader3", 2, "reader B", lck2);
    wr1 = create(swriter3, 2000, 15, "writer3", 2, "writer", lck1);
    start = ctr1000;
    kprintf("-start writer, then sleep 1s. semaphore granted to write (prio 15)\n");
    resume(wr1);
    sleep (1);

    kprintf("-start reader A, then sleep 1s. reader A(prio 45) blocked on the semaphore\n");
    resume(rd1);
    sleep (4);

    kprintf("-start reader B, then sleep 1s. reader A(prio 45) blocked on the semaphore\n");
    resume(rd2);
    sleep (4);

    end = ctr1000;
    kprintf("Total time: %d\n", end-start);

	kprintf("-kill reader A, then sleep 1s\n");
	kill (rd1);
    sleep (1);

    kprintf("-kill reader B, then sleep 1s\n");
	kill (rd2);
	
    sleep(1);
    sleep (5);
}

void task1()
{  
    semaphores();
    sleep(2);

    locks();
    sleep(2);
}

int main()
{
    task1();
    sleep(5);
    shutdown();
}