#include "kevent.h"
#include "schedule.h"
#include "pcb.h"
#include "system.h"
#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

class System;

//KernelEv* (*KernelEv::arr) = new KernelEv*[256];
//KernelEv* KernelEv::arr[256];
KernelEv* (arr)[256];
KernelEv::KernelEv(IVTNo ivtNo):creator(System::running), ivtNo(ivtNo){
	val = 0;
	(arr)[ivtNo] = (KernelEv*)this;
	sem = new KernelSem(0);
}

KernelEv::~KernelEv(){
	if(val == -1){
		val = 0;
		creator->status = READY;
		Scheduler::put(creator);
	}
	(arr)[ivtNo] = NULL;
	delete sem;
}

void KernelEv::wait(){
	if(System::running == creator)
		sem->wait();

	/*if(creator == System::running && val == 0){//ako je kreirana nit i ako nema signala(tj. val==0)
		val = -1;			//-1 znaci da se uslo u blokiranje
		System::blockedDispatch();
	}
	else{
		if(creator == System::running && val == 1){
			val = 0;// gasi se semafor, jer kao da je bio wait, koji je trajao jako kratko
		}
	}*/
}

void KernelEv::signal(){
	if(sem->val() == -1)
		sem->signal();
	/*if(val == -1){//blokirana, opsluzuje se
		val = 0;
		creator->status = READY;
		Scheduler::put(creator);
	}
	else{
		;//val = 1;//stigao signal
	}*/

}

