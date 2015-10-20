// File: pcb.h
#ifndef _pcb_h_
#define _pcb_h_

#include "thread.h"
#include "spqueue.h"
#include "ksemaphr.h"
#include "defs.h"
#include "system.h"
#include "sobject.h"

class KernelSem;
class SleepQueue;

class PCB: public SystemObject{
public:
	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned *stack;
	StackSize stackSize;
	Time timeSlice;
	Time remaining;
	Thread* myThread;
	STATUS status;
	
	static SleepQueue *sleepQueue;

	KernelSem *sem;
	
	static void wrapper();
	void createProcess();
	void initSystemUse();

	void initIdleUse();
	static void wrapperIdle();
	
	friend class KernelSem;
	friend class KernelEv;
	friend class System;
	friend void interrupt mySysCall(...);
	friend void interrupt myTimer(...);

	static unsigned tsp;
	static unsigned tss;
	static unsigned tbp;

	BOOLEAN completed;
	BOOLEAN started;
public:
	

	PCB(Thread *myT, StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	~PCB();


	void start();
	static void sleep(Time timeToSleep);
	void waitToComplete();

	BOOLEAN isMaxPriority();

	BOOLEAN contextSwitch();
	static void initSleep();
	static void terminateSleep();

	void resetTimeSlice();
};

#endif
