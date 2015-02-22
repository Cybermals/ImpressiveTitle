#include "MagixObject.h"

MagixObject::MagixObject()
{
	mObjectNode = 0;
}
MagixObject::~MagixObject()
{
}
const OBJECT_TYPE MagixObject::getType()
{
	return OBJECT_BASIC;
}
const Vector3 MagixObject::getPosition()
{
	if (mObjectNode)return mObjectNode->getPosition();
	return Vector3::ZERO;
}
SceneNode* MagixObject::getObjectNode()
{
	return mObjectNode;
}