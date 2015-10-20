#include "spqueue.h"
#include <stdlib.h>
#include <iostream.h>
#include "schedule.h"

SleepQueue::SleepQueue(){
	first = NULL;
	sem = new KernelSem(0);
}

SleepQueue::~SleepQueue(){
	Elem *t = first;
	sem->signalAll();
	while(first != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	delete sem;
}

void SleepQueue::put(Time timeSlice){
	Elem *t = new Elem();
	t->val = timeSlice;
	t->pcb = System::running;
	Elem *p = first;
	Elem *q = NULL;
	//cout<<endl<<"sleep "<<timeSlice<<endl;
	while(p != NULL && t->val >= p->val){
		t->val -= p->val;
		q = p;
		p = p->next;
	}
	if(q == NULL){//timeSlice ide na 1. mesto
		t->next = first;
		if(first != NULL)
			first->val -= t->val;
		first = t;
	}
	else{//(q != NULL){//timeSlice ide izmedju q i p
		q->next = t;
		t->next = p;
		if(p != NULL)
			p->val -= t->val;
	}
	System::blockedDispatch();
	//sem->wait();
}

void SleepQueue::minusOne(){// subracts 1 from every element in list and removes all those who have 0 remaining time
	if(first == NULL)
		return;
	//System::systemMode = TRUE;
	first->val--;
	Elem *t = first;
	/*while(t != NULL){
		t->val--;
		t = t->next;
	}
	t = first;*/
	while(t != NULL && t->val <= 0){
		//cout<<endl<<"sleep return"<<endl;
		//sem->signal();
		Scheduler::put(t->pcb);
		first = t->next;
		delete t;
		t = first;
	}
	//System::systemMode = FALSE;
}

