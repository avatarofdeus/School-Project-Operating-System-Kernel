#include "event.h"
#include "system.h"
#include "defs.h"
#include <dos.h>
#include "pcb.h"
#include <stdio.h>

Event::Event(IVTNo ivtNo){
	CallInfo info = CallInfo();
	(&info)->isEvent = TRUE;
	(&info)->arg1 = ivtNo;
	(&info)->call = EVENT_CREATE;
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

Event::~Event(){
	CallInfo info = CallInfo();
	(&info)->isEvent = TRUE;
	(&info)->id = myImpl;
	(&info)->call = EVENT_DESTRUCT;
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

void Event::wait(){
	CallInfo info = CallInfo();
	(&info)->isEvent = TRUE;
	(&info)->id = myImpl;
	(&info)->call = EVENT_WAIT;
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

void Event::signal(){
	CallInfo info = CallInfo();
	(&info)->isEvent = TRUE;
	(&info)->id = myImpl;
	(&info)->call = EVENT_SIGNAL;
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

