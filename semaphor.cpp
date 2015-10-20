#include "semaphor.h"
#include "ksemaphr.h"
#include "defs.h"
#include "system.h"
#include <dos.h>

Semaphore::Semaphore(int init){
	lock
	CallInfo info = CallInfo();
	unlock
	(&info)->isSemaphore = TRUE;
	(&info)->arg1 = init;
	(&info)->call = SEMAPHORE_CREATE;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	myImpl = (&info)->id;
	//delete info;
}

Semaphore::~Semaphore(){
	CallInfo info = CallInfo();
	(&info)->isSemaphore = TRUE;
	(&info)->id = myImpl;
	(&info)->call = SEMAPHORE_DESTRUCT;
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

void Semaphore::wait(){
	CallInfo info = CallInfo();
	(&info)->isSemaphore = TRUE;
	(&info)->id = myImpl;
	(&info)->call = SEMAPHORE_WAIT;
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

void Semaphore::signal(){
	lock
	CallInfo info = CallInfo();
	unlock
	(&info)->isSemaphore = TRUE;
	(&info)->id = myImpl;
	(&info)->call = SEMAPHORE_SIGNAL;
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

int Semaphore::val() const{
	CallInfo info = CallInfo();
	(&info)->isSemaphore = TRUE;
	(&info)->id = myImpl;
	(&info)->call = SEMAPHORE_VALUE;
	(&info)->caller = (void*)this;
	unsigned off = FP_OFF((&info));
	unsigned segg = FP_SEG((&info));
	asm{//mozda treba WORD PTR
		mov cx, segg
		mov dx, off
	}
	asm int 60h;
	unsigned val = (&info)->arg1;
	//delete info;
	return val;
}

