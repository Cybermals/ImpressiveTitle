#include "MagixItem.h"

MagixItem::MagixItem(SceneNode *node, const unsigned short &id, const String &meshName)
{
	mObjectNode = node;
	ID = id;
	mesh = meshName;
	hasEffect = false;
}
MagixItem::~MagixItem()
{
}
const OBJECT_TYPE MagixItem::getType()
{
	return OBJECT_ITEM;
}
SceneNode* MagixItem::getObjectNode()
{
	return mObjectNode;
}
const unsigned short MagixItem::getID()
{
	return ID;
}
const String MagixItem::getMesh()
{
	return mesh;
}
const Vector3 MagixItem::getPosition()
{
	if (mObjectNode)return mObjectNode->getPosition();
	return Vector3::ZERO;
}