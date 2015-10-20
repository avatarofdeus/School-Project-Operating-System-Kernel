#include "thread.h"
#include "defs.h"
#include "system.h"
#include <dos.h>
#include "pcb.h"

Thread::Thread(StackSize stackSize, Time timeSlice){
	CallInfo info = CallInfo();
	(&info)->isThread = TRUE;
	(&info)->arg1 = stackSize;
	(&info)->arg2 = timeSlice;
	(&info)->call = THREAD_CREATE;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	myPCB = (&info)->id;
	//delete info;
}

Thread::~Thread(){
	waitToComplete();
	CallInfo info = CallInfo();
	(&info)->id = myPCB;
	(&info)->isThread = TRUE;
	(&info)->call = THREAD_DESTRUCT;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	//delete info;
}

void Thread::start(){
	CallInfo info = CallInfo();
	(&info)->id = myPCB;
	(&info)->isThread = TRUE;
	(&info)->call = THREAD_START;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	//delete info;
}

void Thread::sleep(Time timeToSleep){
	CallInfo info = CallInfo();
	(&info)->isThread = TRUE;
	(&info)->call = THREAD_SLEEP;
	(&info)->arg1 = timeToSleep;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	//delete info;
}

void Thread::waitToComplete(){
	CallInfo info = CallInfo();
	(&info)->id = myPCB;
	(&info)->isThread = TRUE;
	(&info)->call = THREAD_WAIT_TO_COMPLETE;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	//delete info;
}

void Thread::wrapper(Thread* running){
	running->run();
	CallInfo info = CallInfo();
	(&info)->id = running->myPCB;
	(&info)->isThread = TRUE;
	(&info)->call = THREAD_FINISH;
	(&info)->caller = (void*)running;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;//ovde staje put, tako da ne treba delete dole, delete se poziva u systemu
	//delete info;
}

void dispatch(){
	System::dispatch();
}

