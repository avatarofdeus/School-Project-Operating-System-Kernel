// File: vectsobj.h
#ifndef _vectsobj_h_
#define _vectsobj_h_

#include "sobject.h"
#include "defs.h"

typedef unsigned VECTOR_SIZE;
typedef float OCUPIED_RATIO;

#define DEFAULT_VECTOR_SIZE 30
#define DEFAULT_MAX_OCUPATIED_RATIO 0.75f


class VectorSysObj{
	SystemObject **vector;
	VECTOR_SIZE length;
	unsigned ocupied;
	ID iterID;
	ID lastOcupiedID;
	OCUPIED_RATIO ocupiedRatio;

public:
	VectorSysObj(VECTOR_SIZE size = DEFAULT_VECTOR_SIZE, OCUPIED_RATIO ratio = DEFAULT_MAX_OCUPATIED_RATIO);
	~VectorSysObj();
	
	ID put(SystemObject *systemObject);// puts the systemobject and returns id
	SystemObject* get(ID id);//returns systemobject by ID
	SystemObject* remove(ID id);//removes and returns systemobject by ID

	VECTOR_SIZE getLength();//returns the current length of the vector
private:

	BOOLEAN shouldExpand();// checks if it should expand
	BOOLEAN shouldContract();// checks if it should contract
	
	void expand();// expands to biggerSize
	void contract();// contracts to smallerSize
	
	ID getNextID();// returns next free ID, doesn't check if vector is full(might be infinite loop)
	ID getLastOcupiedID();
};

#endif
