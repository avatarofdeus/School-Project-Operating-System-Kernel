#include "ivtentry.h"
#include <dos.h>
#include "kevent.h"
#include "prepare.h"
#include <stdlib.h>
#include <stdio.h>

//IVTEntry* (*IVTEntry::arr) = new IVTEntry* [256];

extern KernelEv *arr[256];

IVTEntry::IVTEntry(IVTNo ivtNo, pointerInterrupt newInterrupt):ivtNo(ivtNo){
	old = getvect(ivtNo);
	setvect(ivtNo, newInterrupt);
}

IVTEntry::~IVTEntry(){
	setvect(ivtNo, old);
	//arr[ivtNo] = NULL;
	(*old)();
}

void IVTEntry::signal(){
	if(arr[ivtNo] != NULL)
		arr[ivtNo]->signal();
}

void IVTEntry::callOld(){
	(*old)();
}

void IVTEntry::restoreInterrupts(){
	//for(IVTNo i = 0; i<256; i++)
	//	delete (arr)[i];
}

void IVTEntry::createIVTNo(IVTNo ivtNo, pointerInterrupt newInterrupt){
	//(arr)[ivtNo] = new IVTEntry(ivtNo, newInterrupt);
}
