// File: system.h
#ifndef _system_h_
#define _system_h_

#include "defs.h"

class VectorSysObj;
class PCB;
class Scheduler;

class System{
public:
	//main
	
	static void initSystem();// initializes the whole system
	static void restoreSystem();// restores the whole system

	static void initTimer();
	static void restoreTimer();
	
	static void initSysCall();
	static void restoreSysCall();
	
	static void createMainPCB();//initializes running | System::running = new PCB();//prazan stek, timeslice =???, null
	static void terminateMainPCB();//deletes the usermainPCB

	static void createKernelPCB();// creates kernel stack and sp, ss and bp
	static void terminateKernelPCB();// dealocates kernel stack

	static void createIdlePCB();//initializes idle pcb
	static void terminateIdlePCB();//stops the idle thread
	
	static void allocateSystemObjects();// creates a data structure for the SystemObjects
	static void deleteSystemObjects();//deletes pcbs, semaphores and events
	
	
	//misc
	
	//static pointerInterrupt replaceRoutine(IVTNo ivtNo, pointerInterrupt newInterrupt);
	

	static const IVTNo IVTNo_TIMER;
	static const IVTNo IVTNo_SYSCALL;

	static void systemDispatch();
	static void blockedDispatch();
	static void dispatch();


	//static void putPCB(PCB*);

	static PCB * volatile running;
	static PCB *kernel;// pcb of kernel thread
	static PCB *idle;// pcb used when none is available
	static VectorSysObj *vectorPCB;
	static VectorSysObj *vectorOtherSysObj;

	static pointerInterrupt oldTimer;
	static pointerInterrupt oldSysCall;

	static BOOLEAN lockFlag;
	static BOOLEAN systemMode;






	static void systemCallExecute();


};

#endif
