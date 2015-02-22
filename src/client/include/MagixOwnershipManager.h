#pragma once

#include "MagixExternalDefinitions.h"
#include "MagixGameStateManager.h"
#include "MagixCritterManager.h"
#include "MagixUnitManager.h"
#include "MagixNetworkManager.h"
#include "MagixChatManager.h"

class MagixOwnershipManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixCritterManager *mCritterManager;
	MagixUnitManager *mUnitManager;
	MagixNetworkManager *mNetworkManager;
	MagixWorld *mWorld;
	MagixChatManager *mChatManager;
	MagixSkyManager *mSkyManager;
	Real ownershipTimer;
	Real spawnTimer;
	Real stationaryCritterTimer;
public:
	MagixOwnershipManager();
	~MagixOwnershipManager();
	void initialize(MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixCritterManager *critterMgr, MagixUnitManager *unitMgr, MagixNetworkManager *networkMgr, MagixWorld *world, MagixChatManager *chatMgr, MagixSkyManager *skyMgr);
	void update(const FrameEvent &evt);
	void updateCritterOwnership(const FrameEvent &evt);
	void updateCritters(const FrameEvent &evt);
	void updateUnits(const FrameEvent &evt);
};
