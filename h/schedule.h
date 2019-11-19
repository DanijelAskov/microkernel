#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

class PCB;

class Scheduler {
public:
	static void put (PCB *);
	static PCB* get ();
};

#endif // _SCHEDULE_H_