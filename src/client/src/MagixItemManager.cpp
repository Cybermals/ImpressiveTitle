#include "MagixItemManager.h"
#include "MagixConst.h"

#include "Ogre.h"
using namespace Ogre;

MagixItemManager::MagixItemManager()
{
	mSceneMgr = 0;
	mDef = 0;
	mEffectsManager = 0;
	itemList.clear();
	itemStash.clear();
}
MagixItemManager::~MagixItemManager()
{
	deleteAllItems();
}
void MagixItemManager::initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixEffectsManager *effectsMgr)
{
	mSceneMgr = sceneMgr;
	mDef = def;
	mEffectsManager = effectsMgr;
	itemList.clear();
}
void MagixItemManager::reset(bool clearStash)
{
	deleteAllItems();
	if (clearStash)
	{
		itemStash.clear();
	}
}
void MagixItemManager::update(const FrameEvent &evt, MagixCamera *camera)
{
	for (vector<MagixItem*>::type::iterator it = itemList.begin(); it != itemList.end(); it++)
	{
		MagixItem *tItem = *it;
		//Range check
		bool isInRange = false;
		if (camera)
		{
			if (camera->getCamera()->isAttached())isInRange = (tItem->getPosition().squaredDistance(camera->getCamera()->getDerivedPosition()) < 1000000);
			else isInRange = (tItem->getPosition().squaredDistance(camera->getCamera()->getPosition()) < 1000000);
		}
		if (isInRange && !tItem->hasEffect)
		{
			mEffectsManager->createItemParticle(tItem->getObjectNode());
			tItem->hasEffect = true;
		}
		else if (!isInRange && tItem->hasEffect)
		{
			mEffectsManager->destroyItemParticleByObjectNode(tItem->getObjectNode());
			tItem->hasEffect = false;
		}
	}
}
void MagixItemManager::createItem(const unsigned short &iID, const String &meshName, const Vector3 &position)
{
	deleteItem(iID);
	Entity *ent = mSceneMgr->createEntity("Item" + StringConverter::toString(iID), meshName + ".mesh");
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(position);
	ent->setQueryFlags(ITEM_MASK);
	ent->getAnimationState("Floor")->setEnabled(true);
	MagixItem *item = new MagixItem(node, iID, meshName);
	itemList.push_back(item);
}
void MagixItemManager::deleteItem(MagixItem *tItem, const vector<MagixItem*>::type::iterator &it)
{
	if (!tItem)return;
	SceneNode *tObjectNode = tItem->getObjectNode();
	if (tObjectNode)
	{
		if (tItem->hasEffect)mEffectsManager->destroyItemParticleByObjectNode(tObjectNode);
		tObjectNode->detachAllObjects();
		mSceneMgr->destroySceneNode(tObjectNode->getName());
	}
	if (mSceneMgr->hasEntity("Item" + StringConverter::toString(tItem->getID())))mSceneMgr->destroyEntity("Item" + StringConverter::toString(tItem->getID()));
	itemList.erase(it);
	delete tItem;
}
void MagixItemManager::deleteItem(const unsigned short &iID)
{
	for (vector<MagixItem*>::type::iterator it = itemList.begin(); it != itemList.end(); it++)
	{
		MagixItem *tItem = *it;
		if (tItem->getID() == iID)
		{
			deleteItem(tItem, it);
			return;
		}
	}
}
void MagixItemManager::deleteAllItems()
{
	while (itemList.size()>0)
	{
		vector<MagixItem*>::type::iterator it = itemList.end();
		it--;
		MagixItem *tItem = *it;
		deleteItem(tItem, it);
	}
}
MagixItem* MagixItemManager::getByObjectNode(SceneNode *node)
{
	for (vector<MagixItem*>::type::iterator it = itemList.begin(); it != itemList.end(); it++)
	{
		MagixItem *tItem = *it;
		if (tItem->getObjectNode() == node)
		{
			return tItem;
		}
	}
	return 0;
}
MagixItem* MagixItemManager::getByID(const unsigned short &iID)
{
	for (vector<MagixItem*>::type::iterator it = itemList.begin(); it != itemList.end(); it++)
	{
		MagixItem *tItem = *it;
		if (tItem->getID() == iID)
		{
			return tItem;
		}
	}
	return 0;
}
const unsigned short MagixItemManager::getNumItems()
{
	return (unsigned short)itemList.size();
}
const unsigned short MagixItemManager::getNextEmptyID(const unsigned short &start)
{
	unsigned short tID = start;
	bool tFound = false;
	while (!tFound)
	{
		tFound = true;
		for (vector<MagixItem*>::type::iterator it = itemList.begin(); it != itemList.end(); it++)
		{
			MagixItem *tItem = *it;
			if (tItem->getID() == tID)
			{
				tFound = false;
				break;
			}
		}
		if (tFound)break;
		else tID++;
	}
	return tID;
}
const vector<MagixItem*>::type MagixItemManager::getItemList()
{
	return itemList;
}
bool MagixItemManager::isStashFull()
{
	return (itemStash.size() >= MAX_STASH);
}
bool MagixItemManager::pushStash(const String &name)
{
	if (itemStash.size() >= MAX_STASH)return false;
	itemStash.push_back(name);
	return true;
}
const String MagixItemManager::getStashAt(const unsigned short &line)
{
	if (line<0 || line >= (int)itemStash.size())return "";
	return itemStash[line];
}
const String MagixItemManager::popStash(const unsigned short &line)
{
	String tItem = "";
	unsigned short tCount = 0;
	for (vector<const String>::type::iterator it = itemStash.begin(); it != itemStash.end(); it++)
	{
		if (tCount == line)
		{
			tItem = *it;
			itemStash.erase(it);
			break;
		}
		tCount++;
	}
	return tItem;
}
const vector<const String>::type MagixItemManager::getStash()
{
	return itemStash;
}
void MagixItemManager::clearStash()
{
	itemStash.clear();
}