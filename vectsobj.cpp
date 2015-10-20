#include "vectsobj.h"
#include "stdlib.h"

VectorSysObj::VectorSysObj(VECTOR_SIZE size, OCUPIED_RATIO ratio){
	vector = new SystemObject*[size];
	length = size;
	ocupied = 0;
	ocupiedRatio = ratio;
	for(int i = 0; i<length; i++)// set all to empty
		vector[i] = NULL;
	iterID = 0;
	lastOcupiedID = 0;
}

VectorSysObj::~VectorSysObj(){
	delete []vector;
	length = ocupied = iterID = 0;
	lastOcupiedID = 0;
}
	
ID VectorSysObj::put(SystemObject *systemObject){
	if(shouldExpand())
		expand();
	ID id = getNextID();
	ocupied++;
	vector[id] = systemObject;
	if(id>lastOcupiedID)
		lastOcupiedID = id;
	return id;
}

SystemObject* VectorSysObj::get(ID id){
	return vector[id];
}

SystemObject* VectorSysObj::remove(ID id){
	if(vector[id] == NULL)
		return NULL;
	SystemObject *temp = vector[id];
	ocupied--;
	vector[id] = NULL;
	lastOcupiedID = getLastOcupiedID();
	if(shouldContract())
		contract();
	return temp;
}

VECTOR_SIZE VectorSysObj::getLength(){
	return length;
}

BOOLEAN VectorSysObj::shouldExpand(){
	return (float)ocupied/length > ocupiedRatio;
}

BOOLEAN VectorSysObj::shouldContract(){
	return (float)lastOcupiedID/length < ocupiedRatio;
}
	
void VectorSysObj::expand(){
	VECTOR_SIZE newLength = 2*length;//expands by factor of 2
	SystemObject **temp = new SystemObject*[newLength];
	for(ID i = 0; i<length; i++)
		temp[i] = vector[i];
	for(i = length; i<newLength; i++)
		temp[i] = NULL;
	delete []vector;
	length = newLength;
	vector = temp;
}

void VectorSysObj::contract(){
	VECTOR_SIZE newLength = lastOcupiedID+10;//cannot contract more than the last
	SystemObject **temp = new SystemObject*[newLength];
	for(ID i = 0; i<newLength; i++)
		temp[i] = vector[i];
	delete []vector;
	length = newLength;
	vector = temp;
}

ID VectorSysObj::getNextID(){
	while(vector[iterID] != NULL)
		iterID = (iterID+1) % length;
	return iterID;
}

ID VectorSysObj::getLastOcupiedID(){
	if(vector[lastOcupiedID] != NULL)
		return lastOcupiedID;
	while(vector[lastOcupiedID] == NULL && lastOcupiedID>=0)
		lastOcupiedID--;
	return lastOcupiedID;
}

