#pragma once

#include "MagixExternalDefinitions.h"
#include "MagixEffectsManager.h"
#include "MagixCamera.h"
#include "MagixItem.h"

using namespace Ogre;

class MagixItemManager
{
protected:
	SceneManager *mSceneMgr;
	MagixExternalDefinitions *mDef;
	MagixEffectsManager *mEffectsManager;
	vector<MagixItem*>::type itemList;
	vector<const String>::type itemStash;
public:
	MagixItemManager();
	~MagixItemManager();
	void initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixEffectsManager *effectsMgr);
	void reset(bool clearStash = false);
	void update(const FrameEvent &evt, MagixCamera *camera = 0);
	void createItem(const unsigned short &iID, const String &meshName, const Vector3 &position);
	void deleteItem(MagixItem *tItem, const vector<MagixItem*>::type::iterator &it);
	void deleteItem(const unsigned short &iID);
	void deleteAllItems();
	MagixItem* getByObjectNode(SceneNode *node);
	MagixItem* getByID(const unsigned short &iID);
	const unsigned short getNumItems();
	const unsigned short getNextEmptyID(const unsigned short &start);
	const vector<MagixItem*>::type getItemList();
	bool isStashFull();
	bool pushStash(const String &name);
	const String getStashAt(const unsigned short &line);
	const String popStash(const unsigned short &line);
	const vector<const String>::type getStash();
	void clearStash();
};
