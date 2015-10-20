// File: spqueue.h
#ifndef _spqueue_h_
#define _spqueue_h_

#include "thread.h"
#include "semaphor.h"
#include "ksemaphr.h"

class Semaphore;
class KernelSem;

class SleepQueue{
	struct Elem{
		Time volatile val;
		Elem *next;
		PCB *pcb;
	};
	Elem *first;
	KernelSem *sem;
public:
	SleepQueue();
	~SleepQueue();

	void put(Time timeSlice);
	void minusOne();		// substracts all the numbers in pqueue by 1, and activates the signal
};

#endif
