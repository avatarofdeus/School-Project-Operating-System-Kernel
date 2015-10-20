#include "queue.h"
#include "pcb.h"
#include <stdlib.h>

Queue::Queue(){
	len = 0;
	first = end = NULL;
}

Queue::~Queue(){
	len = 0;
	Elem *t = first;
	while(t != NULL){
		t = first;
		first = first->next;
		delete t;
	}
	first = end = NULL;
}

void Queue::insert(PCB *pcb){
	if(len++ == 0){
		first = end = new Elem();
	}
	else{
		end = end->next = new Elem();
	}
	end->val = pcb;
	end->next = NULL;
}

PCB* Queue::remove(){
	if(len == 0)
		return NULL;
	len--;
	PCB *p = first->val;
	Elem *t = first;
	first = first->next;
	delete t;
	return p;
}

int Queue::size() const{
	return len;
}

