// File: ksemaphr.h
#ifndef _ksemaphr_h_
#define _ksemaphr_h_

#include "queue.h"
#include "sobject.h"
class Queue;

class KernelSem: public SystemObject{
	volatile int value;
	Queue *qb;
public:
  	KernelSem(int init);
	~KernelSem();

	void wait();
	void signal();

	void signalAll();

 	int val() const;
};

#endif
