#pragma once

#include "MagixUnit.h"

class MagixIndexedUnit : public MagixUnit
{
protected:
	MagixIndexedUnit *mPrevUnit;
	unsigned short index;
public:
	bool hasMarker;
	bool isFriend;
	bool isBlocked;
	bool isAdmin;
	bool isMod;
	bool isWounded;
	MagixIndexedUnit(const unsigned short &i, MagixIndexedUnit *prev);
	~MagixIndexedUnit();
	const unsigned short getIndex();
	MagixIndexedUnit* getPrevious();
	void setPrevious(MagixIndexedUnit *prev);
	bool isIndexedUnit();
};
