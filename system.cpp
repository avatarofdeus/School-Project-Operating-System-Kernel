#include "system.h"
#include <dos.h>
#include "pcb.h"
#include "defs.h"
#include "ivtentry.h"
#include <stdlib.h>
#include "schedule.h"
#include "vectsobj.h"
#include "kevent.h"
#include "ksemaphr.h"
#include <stdio.h>
#include <iostream.h>

class KernelEv;
class KernelSem;

void interrupt mySysCall(...);
void interrupt myTimer(...);

const IVTNo System::IVTNo_TIMER = 0x08;
const IVTNo System::IVTNo_SYSCALL = 0x60;
BOOLEAN System::systemMode = FALSE;
BOOLEAN System::lockFlag = UNLOCKED;
unsigned tsp;
unsigned tss;
unsigned tbp;
unsigned toff;
unsigned tseg;

VectorSysObj *System::vectorPCB = NULL;
VectorSysObj *System::vectorOtherSysObj = NULL;

CallInfo *callInfo;

PCB * volatile System::running = NULL;
PCB *System::kernel = NULL;// pcb of kernel thread
PCB *System::idle = NULL;// pcb used when none is available

pointerInterrupt System::oldTimer = myTimer;
pointerInterrupt System::oldSysCall = mySysCall;


extern void tick();

void System::initSystem(){
	//createMainPCB has to be before timer, if timer ticks, running might be NULL, so it wont work
	createMainPCB();//creates the main PCB

	createKernelPCB();//creates kernel stack
	createIdlePCB();//initializes idle stack

	allocateSystemObjects();

	PCB::initSleep();

	initTimer();//timer init
	initSysCall();//system interrupt init
}

void System::restoreSystem(){
	//sve niti ovde da se zavrse, userPCB included
	//svi semafori da se izbrisu
	//svi eventi da se izbrisu
	
	//restaurirati sve prekidne rutine
	deleteSystemObjects();
	
	restoreSysCall();//restores the previous routine
	restoreTimer();//restores the previous timer

	IVTEntry::restoreInterrupts();

	PCB::terminateSleep();

	terminateIdlePCB();
	terminateKernelPCB();

	terminateMainPCB();
}

void System::initTimer(){
	oldTimer = getvect(IVTNo_TIMER);
	setvect(IVTNo_TIMER, myTimer);
}

void System::restoreTimer(){
	setvect(IVTNo_TIMER, oldTimer);
}

void System::initSysCall(){
	oldSysCall = getvect(IVTNo_SYSCALL);
	setvect(IVTNo_SYSCALL, mySysCall);
}

void System::restoreSysCall(){
	setvect(IVTNo_SYSCALL, oldSysCall);
}

void System::createMainPCB(){
	running = new PCB(NULL, 0, 0);//when system or timer called, sp, bp and ss will be initialized
}

void System::terminateMainPCB(){
	delete running;
}

void System::createKernelPCB(){
	kernel = new PCB(NULL);//stacksize is default; allocated time is unimportant, not used
	kernel->initSystemUse();
}

void System::createIdlePCB(){
	idle = new PCB(NULL, 32, 1);
	idle->initIdleUse();
}

void System::allocateSystemObjects(){
	//ne mora lock i unlock, jer ide pre timera
	vectorPCB = new VectorSysObj();
	vectorOtherSysObj = new VectorSysObj();
}

void System::deleteSystemObjects(){
	VECTOR_SIZE lengthPCB = vectorPCB->getLength();
	for(ID i = 0; i<lengthPCB; i++){//idle is not in this list
		SystemObject *sO = vectorPCB->get(i);
		if(sO != NULL)
			((PCB*)sO)->waitToComplete();
	}
	for(i = 0; i<lengthPCB; i++){//idle is not in this list
			delete vectorPCB->remove(i);
	}
	VECTOR_SIZE lengthOtherSysObj = vectorOtherSysObj->getLength();
	for(i = 0; i<lengthOtherSysObj; i++){
		delete vectorOtherSysObj->remove(i);
	}
	delete vectorPCB;
	delete vectorOtherSysObj;
}

void System::terminateIdlePCB(){
	delete idle;
}

void System::terminateKernelPCB(){
	delete kernel;
}
/*
pointerInterrupt System::replaceRoutine(IVTNo ivtNo, pointerInterrupt newInterrupt){
	pointerInterrupt pI = getvect(ivtNo);
	setvect(ivtNo, newInterrupt);
	return pI;
}
*/

void System::systemDispatch(){
	if(running != idle){
		Scheduler::put(running);
	}
	running = Scheduler::get();
	if(running == NULL)
		running = idle;
	running->remaining = running->timeSlice;
}

void System::blockedDispatch(){
	running = Scheduler::get();
	if(running == NULL)
		running = idle;
	running->remaining = running->timeSlice;
}

void System::dispatch(){
	CallInfo info = CallInfo();
	(&info)->call = DISPATCH;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	//delete info;
}
/*
void System::putPCB(PCB*pcb){
	vectorPCB(put);
}
*/
void interrupt myTimer(...){
	//cout<<endl<<"timer"<<endl;
	tick();
	(*System::oldTimer)();
	PCB::sleepQueue->minusOne();

	if(System::systemMode || System::running->isMaxPriority()){// in systemMode there arent any other operations
		return;
	}
	System::running->remaining--;
	if(System::running->remaining <= 0 && System::lockFlag == UNLOCKED){//out of time, contextSwitch == running->remaining<=0
		asm{
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
		System::running->sp = tsp;
		System::running->ss = tss;
		System::running->bp = tbp;

		if(System::running != System::idle){
			Scheduler::put(System::running);
		}
		System::running = Scheduler::get();
 		if(System::running == NULL)
 			System::running = System::idle;

		tsp = System::running->sp;
		tss = System::running->ss;
		tbp = System::running->bp;
		asm{
			mov sp, tsp // restore sp
			mov ss, tss
			mov bp, tbp
		}
		System::running->remaining = System::running->timeSlice;
	}
}

void interrupt mySysCall(...){
	System::systemMode = TRUE;

	asm{
		// cuva sp
		mov tsp, sp
		mov tss, ss
		mov tbp, bp
		mov tseg, cx
		mov toff, dx
	}
	System::running->sp = tsp;
	System::running->ss = tss;
	System::running->bp = tbp;

	tsp = System::kernel->sp;
	tss = System::kernel->ss;
	tbp = System::kernel->bp;
	asm{
		mov sp, tsp // restore kernel sp
		mov ss, tss
		mov bp, tbp
	}
	
	callInfo = (CallInfo*)MK_FP(tseg, toff);
	System::systemCallExecute();

	//Scheduler::put(System::running);
	//System::running = Scheduler::get();

	//System::systemDispatch();

	tsp = System::running->sp;
	tss = System::running->ss;
	tbp = System::running->bp;

	asm{
		mov sp, tsp
		mov ss, tss
		mov bp, tbp
	}

	System::systemMode = FALSE;
}

void System::systemCallExecute(){
	if(callInfo->call == DISPATCH){//done
		;
		System::systemDispatch();
		
	}else if(callInfo->call == BLOCKED_DISPATCH){//done
		System::blockedDispatch();
		

	//THREAD

	}else if(callInfo->call == THREAD_CREATE && callInfo->isThread){//done
		callInfo->id = vectorPCB->put(new PCB((Thread*)callInfo->caller, callInfo->arg1, callInfo->arg2));

	}else if(callInfo->call == THREAD_START && callInfo->isThread){//done
		((PCB*)vectorPCB->get(callInfo->id))->start();

	}else if(callInfo->call == THREAD_WAIT_TO_COMPLETE && callInfo->isThread){//done
		((PCB*)vectorPCB->get(callInfo->id))->waitToComplete();

	}else if(callInfo->call == THREAD_SLEEP && callInfo->isThread){//done
		PCB::sleep(callInfo->arg1);
	
	}else if(callInfo->call == THREAD_DESTRUCT && callInfo->isThread){//done
		PCB*p = (PCB*)vectorPCB->remove(callInfo->id);
		delete p;
	
	}else if(callInfo->call == THREAD_FINISH && callInfo->isThread){//done
		//running je ovo pozvao
		//PCB *pcb = (PCB*)vectorPCB->get(callInfo->id);
		running->sem->signalAll();
		delete []running->stack;
		running->stack = NULL;
		running->status = DONE;
		running->completed = TRUE;
		blockedDispatch();//doneDispatch();// treba da se obrise ovaj PCB i da se pusti neki drugi thread
		//delete callInfo;
	//SEMAPHORE


	}else if(callInfo->call == SEMAPHORE_CREATE && callInfo->isSemaphore){//done
		callInfo->id = vectorOtherSysObj->put(new KernelSem(callInfo->arg1));
	
	}else if(callInfo->call == SEMAPHORE_WAIT && callInfo->isSemaphore){//done
		((KernelSem*)vectorOtherSysObj->get(callInfo->id))->wait();
	
	}else if(callInfo->call == SEMAPHORE_SIGNAL && callInfo->isSemaphore){//done
		((KernelSem*)vectorOtherSysObj->get(callInfo->id))->signal();
	
	}else if(callInfo->call == SEMAPHORE_VALUE && callInfo->isSemaphore){//done
		callInfo->arg1 = ((KernelSem*)vectorOtherSysObj->get(callInfo->id))->val();
	
	}else if(callInfo->call == SEMAPHORE_DESTRUCT && callInfo->isSemaphore){//done
		delete vectorOtherSysObj->remove(callInfo->id);
	
	}


	//EVENT

	else if(callInfo->call == EVENT_CREATE && callInfo->isEvent){//done
		callInfo->id = vectorOtherSysObj->put(new KernelEv(callInfo->arg1));
	
	}else if(callInfo->call == EVENT_WAIT && callInfo->isEvent){//done
		((KernelEv*)vectorOtherSysObj->get(callInfo->id))->wait();

	}else if(callInfo->call == EVENT_SIGNAL && callInfo->isEvent){//done
		((KernelEv*)vectorOtherSysObj->get(callInfo->id))->signal();
	
	}else if(callInfo->call == EVENT_DESTRUCT && callInfo->isEvent){//done
		delete vectorOtherSysObj->remove(callInfo->id);
	
	}
	//System::systemDispatch();
}

