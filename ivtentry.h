// File: ivtentry.h
#ifndef _ivtentry_h_
#define _ivtentry_h_

#include "event.h"
#include "defs.h"

class KernelEv;

class IVTEntry{

	IVTNo ivtNo;
	pointerInterrupt old;

public:
	IVTEntry(IVTNo ivtNo, pointerInterrupt newInterrupt);
	~IVTEntry();

	//static IVTEntry* *arr;

	void signal();
	void callOld();

	static void restoreInterrupts();
	static void createIVTNo(IVTNo ivtNo, pointerInterrupt newInterrupt);
};

#endif
