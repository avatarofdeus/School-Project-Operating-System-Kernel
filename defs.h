// File: defs.h
#ifndef _defs_h_
#define _defs_h_

typedef unsigned char IVTNo;
typedef void interrupt(*pointerInterrupt)(...);

typedef unsigned ID;

typedef unsigned BOOLEAN;

enum{FALSE = 0, TRUE = 1};

enum STATUS{READY, DONE, BLOCKED, SLEEP, CREATED, SYSTEM, IDLE};

enum CALLS{DISPATCH, BLOCKED_DISPATCH, THREAD_CREATE, THREAD_START, THREAD_WAIT_TO_COMPLETE, THREAD_SLEEP, THREAD_DESTRUCT, THREAD_FINISH, SEMAPHORE_CREATE, SEMAPHORE_WAIT, SEMAPHORE_SIGNAL, SEMAPHORE_VALUE, SEMAPHORE_DESTRUCT, EVENT_CREATE, EVENT_WAIT, EVENT_SIGNAL, EVENT_DESTRUCT};

struct CallInfo{
	ID id;
	BOOLEAN isThread;
	BOOLEAN isEvent;
	BOOLEAN isSemaphore;
	CALLS call;
	void *caller;
	unsigned long arg1;
	unsigned long arg2;

	CallInfo(){
		isThread = isEvent = isSemaphore = FALSE;
	}
};

#define lock System::lockFlag = LOCKED;

#define unlock System::lockFlag = UNLOCKED;\
	if(System::running->contextSwitch())\
		System::dispatch();

enum {LOCKED, UNLOCKED};

#endif
