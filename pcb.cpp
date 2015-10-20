#include "pcb.h"
#include <dos.h>
#include "defs.h"
#include <stdlib.h>
#include "schedule.h"
#include "spqueue.h"
#include <iostream.h>

SleepQueue *PCB::sleepQueue = NULL;

/*void PCB::wrapper(){
	Kernel::running->myThread->run();
	lock
	Kernel::running->sem.signalAll();
	delete []Kernel::running->stack;
	Kernel::running->stack = new unsigned char[24];//alocira se mnogo manji stek, zbog potrebe funkcije blockedDispatch()
	tsp = FP_OFF(Kernel::running->stack + 23);
	tss = FP_SEG(Kernel::running->stack + 23);
	asm{
		mov sp, tsp // restore sp
		mov ss, tss
	}
	Kernel::running->status = DONE;
	blockedDispatch();//doneDispatch();// treba da se obrise ovaj PCB i da se pusti neki drugi thread
	
}
*/
void PCB::createProcess(){
	//ovde moram da ubacim u registar pcbova systema, NE, ubacen je u Thread konstruktoru
	stack = new unsigned[stackSize];
	stack[stackSize-1] = FP_SEG(myThread);
	stack[stackSize-2] = FP_OFF(myThread);

	stack[stackSize-5] = 0x200; //setovan I fleg u
 	// pocetnom PSW-u za nit
	stack[stackSize-6] = FP_SEG(Thread::wrapper);
	stack[stackSize-7] = FP_OFF(Thread::wrapper);
	bp = sp = FP_OFF(stack + stackSize-16); //svi sacuvani registri
	// pri ulasku u interrupt
	// rutinu
	ss = FP_SEG(stack + stackSize-16);
	status = READY;
	completed = FALSE;
}

void PCB::initSystemUse(){
	//nema lock i unlock, jer myTimer nije inicijalizovan
	stack = new unsigned[stackSize];
	sp = FP_OFF(stack + stackSize); //svi sacuvani registri
	ss = FP_SEG(stack + stackSize);
	bp = FP_OFF(stack + stackSize);
	status = SYSTEM;
}

void PCB::wrapperIdle(){
	while(1);
}

void PCB::initIdleUse(){
	//nema lock i unlock, jer myTimer nije inicijalizovan
	stack = new unsigned[stackSize];
	stack[stackSize-1] = 0x200; //setovan I fleg u
 	// pocetnom PSW-u za nit
	stack[stackSize-2] = FP_SEG(wrapperIdle);//mozda treba & kao adresa
	stack[stackSize-3] = FP_OFF(wrapperIdle);
	sp = FP_OFF(stack + stackSize-12); //svi sacuvani registri
	ss = FP_SEG(stack + stackSize-12);
	bp = FP_OFF(stack + stackSize-12);
	status = IDLE;
}

PCB::PCB(Thread *myThread, StackSize stackSize, Time timeSlice): stackSize(stackSize), timeSlice(timeSlice), myThread(myThread){
	status = CREATED;
	stack = NULL;
	started = FALSE;
	sem = new KernelSem(0);
	completed = FALSE;
}

PCB::~PCB(){
	delete []stack;
	delete sem;
}

void PCB::start(){
	//pravim stack(da stedim prostor, kreira se onda samo kada je to potrebno), stek pravim sa koeficijentom /2
	//stavljam wrapper na stack od this
	if(!started){
		createProcess();
		Scheduler::put(this);
		started = TRUE;
	}
}

void PCB::sleep(Time timeToSleep){
	System::running->status = SLEEP;
	sleepQueue->put(timeToSleep);
	System::running->status = READY;
}

void PCB::waitToComplete(){//programer mora da pazi da ne pozove ovo za samu nit
	if(completed)
		return;
	sem->wait();
}

BOOLEAN PCB::isMaxPriority(){
	return !timeSlice;
}

BOOLEAN PCB::contextSwitch(){
	return remaining <= 0 && !isMaxPriority();
}

void PCB::initSleep(){
	sleepQueue = new SleepQueue();
}

void PCB::terminateSleep(){
	delete sleepQueue;
}

void PCB::resetTimeSlice(){
	remaining = timeSlice;
}
