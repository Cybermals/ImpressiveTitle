#pragma once

#include "MagixNetworkDefines.h"
#include "MagixExternalDefinitions.h"
#include "MagixGameStateManager.h"
#include "MagixEffectsManager.h"
#include "MagixCollisionManager.h"
#include "MagixCamera.h"
#include "MagixSoundManager.h"
#include "MagixSkyManager.h"
#include "MagixWorld.h"
#include "MagixUnitManager.h"
#include "MagixItemManager.h"
#include "MagixGUI.h"
#include "MagixNetworkManager.h"
#include "MagixOwnershipManager.h"
#include "MagixCampaignManager.h"

using namespace Ogre;

class MagixHandler
{
protected:
	SceneManager *mSceneMgr;
	RenderWindow *mWindow;
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixCamera *mCamera;
	MagixItemManager *mItemManager;
	MagixCritterManager *mCritterManager;
	MagixUnitManager *mUnitManager;
	MagixSkyManager *mSkyManager;
	MagixGUI *mGui;
	MagixWorld *mWorld;
	MagixSoundManager *mSoundManager;
	MagixNetworkManager *mNetworkManager;
	MagixEffectsManager *mEffectsManager;
	MagixCollisionManager *mCollisionManager;
	MagixOwnershipManager *mOwnershipManager;
	MagixCampaignManager *mCampaignManager;

	String mDebugText;
	bool initialized;

public:
	MagixHandler();
	~MagixHandler();
	void initialize(SceneManager *sceneMgr, RenderWindow *window);
	void shutdown();
	void resetScene(bool startScene = false, const String &mapChange = "");
	bool update(const FrameEvent &evt);
	void updateEverything(const FrameEvent &evt);
	void updateCommand();
	void updateMapChange(const FrameEvent &evt);
	void updateCredits(const FrameEvent &evt);
	void updateGameOver(const FrameEvent &evt);
	void updateLogo(const FrameEvent &evt);
	void updateStartScreen(const FrameEvent &evt);
	void updateCharScreen(const FrameEvent &evt);
	MagixPlayer* getPlayer();
	String getDebugText();
	MagixGUI* getMagixGUI();
	MagixInputManager* getInputManager();
	MagixUnitManager* getUnitManager();
	MagixCamera* getCamera();
	MagixExternalDefinitions* getExternalDefinitions();
	MagixNetworkManager* getNetworkManager();
	unsigned short getCameraMode();
	unsigned short getInputMode();
	MagixGameStateManager* getGameState();
	MagixEffectsManager* getEffectsManager();
};
