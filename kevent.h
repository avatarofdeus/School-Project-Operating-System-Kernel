// File: kevent.h
#ifndef _kevent_h_
#define _kevent_h_

#include "defs.h"
#include "event.h"
#include "schedule.h"
#include "ivtentry.h"
#include "sobject.h"

class KernelSem;
class KernelEv: public SystemObject{
	PCB* const creator;	// za vreme konstrukcije objekta, ovde se pamti running
	int val;	//vrednost semafora na kojem se blokira, moze samo 1 nit da bude blokirana(creator)
	IVTNo ivtNo;	//broj ulaza u ivt

	//static KernelEv* (arr)[256];
	friend class IVTEntry;
	KernelSem *sem;
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();

	void wait();
	void signal();
};

#endif
