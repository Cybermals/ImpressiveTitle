#include "MagixIndexedUnit.h"

MagixIndexedUnit::MagixIndexedUnit(const unsigned short &i, MagixIndexedUnit *prev)
{
	mPrevUnit = prev;
	index = i;
	hasMarker = false;
	isFriend = false;
	isBlocked = false;
	isAdmin = false;
	isMod = false;
	isWounded = false;
}
MagixIndexedUnit::~MagixIndexedUnit()
{
}
const unsigned short MagixIndexedUnit::getIndex()
{
	return index;
}
MagixIndexedUnit* MagixIndexedUnit::getPrevious()
{
	return mPrevUnit;
}
void MagixIndexedUnit::setPrevious(MagixIndexedUnit *prev)
{
	mPrevUnit = prev;
}
bool MagixIndexedUnit::isIndexedUnit()
{
	return true;
}
