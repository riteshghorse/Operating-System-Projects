#ifndef _LAB1_H_
#define _LAB1_H_

#define EXPDISTSCHED 1
#define LINUXSCHED 2

extern int schedclass;
void setschedclass(int sched_class);
int getschedclass();
int getnextexpproc(int expval);
int getlastexpproc(int process);
#endif
