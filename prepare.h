// File: prepare.h
#ifndef _prepare_h_
#define _prepare_h_

#include "ivtentry.h"
#include "defs.h"
#include <stdio.h>
#include "pcb.h"
#include "schedule.h"

#define PREPAREENTRY(ivtNo, old)\
	void interrupt Ievent##ivtNo(...);\
	\
	static IVTEntry Ientry##ivtNo = IVTEntry(ivtNo, Ievent##ivtNo);\
	\
	extern unsigned tbp, tss, tsp;\
	\
	void interrupt Ievent##ivtNo(...){\
		Ientry##ivtNo.signal();\
		if(old)\
			Ientry##ivtNo.callOld();\
		if(System::systemMode)\
			return;\
		tsp = _SP;\
		tss = _SS;\
		tbp = _BP;\
		\
		System::running->sp = tsp;\
		System::running->ss = tss;\
		System::running->bp = tbp;\
		if(System::running != System::idle){\
			Scheduler::put(System::running);\
		}\
		System::running = Scheduler::get();\
		if(System::running == NULL)\
			System::running = System::idle;\
		tsp = System::running->sp;\
		tss = System::running->ss;\
		tbp = System::running->bp;\
		_SP = tsp;\
		_SS = tss;\
		_BP = tbp;\
		System::running->resetTimeSlice();\
	}

#endif
