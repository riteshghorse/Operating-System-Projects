#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab1.h>
#include <math.h>

int schedclass;

void setschedclass(int sched_class)
{
	schedclass = sched_class;
}

int getschedclass()
{
	return schedclass;
}
