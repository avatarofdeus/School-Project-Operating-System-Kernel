// File: queue.h
#ifndef _queue_h_
#define _queue_h_

#include "pcb.h"

class PCB;

class Queue{
	struct Elem{
		PCB *val;
		Elem *next;
	};

	Elem *first, *end;
	int len;
public:
	Queue();
	~Queue();

	void insert(PCB *pcb);
	PCB* remove();

	int size() const;
};

#endif
