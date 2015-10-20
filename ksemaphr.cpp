#include "ksemaphr.h"
#include "pcb.h"
#include "schedule.h"
#include "queue.h"
#include "defs.h"
	
KernelSem::KernelSem(int init): value(init){
	qb = new Queue();
}
  
KernelSem::~KernelSem(){
	delete qb;
}

void KernelSem::wait(){
	if(--value<0){
		qb->insert((PCB*)System::running);
		System::blockedDispatch();
	}
}

void KernelSem::signal(){
	if(value++<0){
		PCB *p = qb->remove();
		if(p){
			p->status = READY;
			Scheduler::put(p);
		}// ako je p == NULL, prazna je lista onih na cekanju
	}
}

int KernelSem::val() const{
	return value;
}

void KernelSem::signalAll(){
	while(value<0)
		signal();
}

