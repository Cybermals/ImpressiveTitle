#ifndef __MagixHandler_h_
#define __MagixHandler_h_

enum QueryFlags
{
	OBJECT_MASK = 1<<0,
    UNIT_MASK = 1<<1,
    SKY_MASK = 1<<2,
	WORLDOBJECT_MASK = 1<<3,
	EFFECT_MASK = 1<<4,
	ITEM_MASK = 1<<5,
	CRITTER_MASK = 1<<6
};

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
	MagixHandler()
	{
		mSceneMgr = 0;
		mWindow = 0;
		mDebugText = "";
		initialized = false;

		mDef = new MagixExternalDefinitions();
		mGameStateManager = new MagixGameStateManager();
		mCamera = new MagixCamera();
		mItemManager = new MagixItemManager();
		mCritterManager = new MagixCritterManager();
		mUnitManager = new MagixUnitManager();
		mSkyManager = new MagixSkyManager();
		mGui = new MagixGUI();
		mWorld = new MagixWorld();
		mSoundManager = new MagixSoundManager();
		mNetworkManager = new MagixNetworkManager();
		mEffectsManager = new MagixEffectsManager();
		mCollisionManager = new MagixCollisionManager();
		mOwnershipManager = new MagixOwnershipManager();
		mCampaignManager = new MagixCampaignManager();
	}
	~MagixHandler()
	{
		delete mUnitManager;
		delete mCampaignManager;
		delete mOwnershipManager;
		delete mNetworkManager;
		delete mGui;
		delete mCritterManager;
		delete mWorld;
		delete mSkyManager;
		delete mSoundManager;
		delete mCamera;
		delete mItemManager;
		delete mEffectsManager;
		delete mCollisionManager;
		delete mGameStateManager;
		delete mDef;
	}
	void initialize(SceneManager *sceneMgr, RenderWindow *window)
	{
		if(mSceneMgr)return;
		mSceneMgr = sceneMgr;
		mWindow = window;

		mDef->initialize();
		mGameStateManager->initialize();
		mEffectsManager->initialize(sceneMgr);
		mCollisionManager->initialize(sceneMgr);
		mCamera->initialize(sceneMgr,getPlayer(),mCollisionManager);
		mSoundManager->initialize(mCamera);
		mSkyManager->initialize(sceneMgr, mDef, mCamera, mSoundManager);
		mItemManager->initialize(sceneMgr, mDef, mEffectsManager);
		mWorld->initialize(sceneMgr, window, mDef, mGameStateManager, mSoundManager, mCollisionManager, mSkyManager);
		mCritterManager->initialize(sceneMgr, mDef, mGameStateManager, mEffectsManager, mCollisionManager, mSoundManager, mWorld, mCamera);
		mUnitManager->initialize(sceneMgr, mDef,mGameStateManager,mWorld,mEffectsManager,mCollisionManager,mSoundManager,mItemManager,mCritterManager,mCamera);
		mNetworkManager->initialize(mDef,mGameStateManager,mItemManager,mCritterManager,mUnitManager,mWorld,mSkyManager,mGui->getChatManager(),mGui->getAlertBox(),mGui->getCharScreenManager());
		mOwnershipManager->initialize(mDef,mGameStateManager,mCritterManager,mUnitManager,mNetworkManager,mWorld,mGui->getChatManager(),mSkyManager);
		mGui->initialize(sceneMgr, window, mDef, mGameStateManager, mItemManager, mCritterManager, mUnitManager, mNetworkManager, mCamera, mSoundManager, mEffectsManager, mCollisionManager);
		mCampaignManager->initialize(mDef, mGameStateManager, mGui, mUnitManager, mWorld, mSkyManager, mCamera, mSoundManager, mCollisionManager, mEffectsManager);

		initialized = true;
	}
	void shutdown()
	{
		if(!initialized)return;
		if(!mGameStateManager->isCampaign() && mGameStateManager->getCampaignName()=="" && mWorld->getWorldName()!="" && mGameStateManager->isInGame())
			mDef->saveSavePoint(mWorld->getWorldName(),Vector2(getPlayer()->getPosition().x,getPlayer()->getPosition().z),mGameStateManager->getDimension());
		mGui->saveSettings();
		mDef->saveHotkeys("Hotkeys.cfg");
	}
	void resetScene(bool startScene=false, const String &mapChange="")
	{
		if(!startScene)mSoundManager->reset();
		mSkyManager->resetSky();
		mCollisionManager->reset();
		if(!startScene)
		{
			if(mapChange!="")mWorld->loadWorld(mapChange);
			else mWorld->reset();
			mItemManager->reset();
			mCritterManager->reset();
			mUnitManager->reset();
		}
		mCamera->reset(startScene);
		mNetworkManager->reset();
		mGui->reset();
		mEffectsManager->reset();
	}
	bool update(const FrameEvent &evt)
	{
		bool contFlag = true;

		switch(mGameStateManager->getGameState())
		{
			case GAMESTATE_INITIALIZE:
				mGui->resetLogo();
				mSoundManager->playMusic(MUSIC_TITLE);
				mGameStateManager->setGameState(GAMESTATE_LOGO);
			break;
			case GAMESTATE_LOGO:
				updateLogo(evt);
			break;
			case GAMESTATE_STARTSCREEN:
				updateStartScreen(evt);
			break;
			case GAMESTATE_STARTCAMPAIGN:
				mSkyManager->showSky(true);
				mEffectsManager->reset();
				mCollisionManager->reset();
				mCampaignManager->loadCampaign();
				mCamera->reset();
				mCamera->setCameraMode(CAMERA_LOCKED);
				mSkyManager->resetSky();
				mGui->reset();
				mUnitManager->setShouldNameTagsBeVisible(false);
				mGui->getMiniMap()->updatePortalMap();
				updateEverything(evt);
				mGui->showBackdropOverlay(false);
				mGui->showMouse(false);
				mGameStateManager->setGameState(GAMESTATE_INGAMECAMPAIGN);
				mGui->showGUI(true);
				mGui->showChatOverlay(true,false);
				mGui->setChatChannel(0);
			break;
			case GAMESTATE_STARTCHARSCREEN:
				if(mCamera->getCamera()->isAttached())mCamera->getActiveCamNode()->detachObject(mCamera->getCamera());
				mCamera->getCamera()->setPosition(3200,120,2770);
				mCamera->getCamera()->setOrientation(Quaternion(Degree(-40),Vector3::UNIT_Y));
				mSkyManager->showSky(true);
				mSkyManager->setDayTime(1810);
				mSkyManager->setWeatherTime(0);
				mSkyManager->setWeatherCycle("");
				mSkyManager->update(evt);
				mWorld->loadWorld(MAP_DEFAULT);
				mCollisionManager->destroyAllPortals();
				mCollisionManager->destroyAllGates();
				mWorld->update();
				mGui->showCharScreenOverlay(true);
				mGui->getCharScreenManager()->reset(true);
				mGui->loadChar();
				CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0), "Bloom", mGui->isBloom());
				mGameStateManager->setGameState(GAMESTATE_CHARSCREEN);
			break;
			case GAMESTATE_CHARSCREEN:
				updateCharScreen(evt);
			break;
			case GAMESTATE_RETURNSTARTSCREEN:
				if(mGameStateManager->getGameStarted() && mGameStateManager->getCampaignName()=="" && mWorld->getWorldName()!="" && getPlayer()->getObjectNode())
					mDef->saveSavePoint(mWorld->getWorldName(),Vector2(getPlayer()->getPosition().x,getPlayer()->getPosition().z),mGameStateManager->getDimension());
				mWorld->unloadWorld();
				mSkyManager->showSky(false);
				mSkyManager->resetSky();
				mCamera->reset();
				mCollisionManager->reset();
				mItemManager->reset(true);
				mCritterManager->reset();
				mUnitManager->reset(true);
				mEffectsManager->reset(true);
				mGui->reset();
				mGui->showGUI(false);
				mGui->showStartScreenOverlay(true);
				mGui->setBackdrop("GUIMat/Backdrop1",true);
				mGui->showBackdropOverlay(true,false);
				mGui->showOptionsOverlay(false);
				mGameStateManager->setGameState(GAMESTATE_STARTSCREEN);
				mGameStateManager->setGameStarted(false);
			break;
			case GAMESTATE_STARTCONNECTION:
				mGui->showCharScreenOverlay(false);
				mGui->showBackdropOverlay(true,true);
				mSkyManager->setDayTime(500);
				mUnitManager->setShouldNameTagsBeVisible(true);
				getPlayer()->pushDefaultAttackList("Dash");
				getPlayer()->pushDefaultAttackList("Slash");
				getPlayer()->pushDefaultAttackList("Roundhouse");
				if(!getPlayer()->getWingEnt())getPlayer()->pushDefaultAttackList("Somersault");
				else getPlayer()->pushDefaultAttackList("Wingslash");
				getPlayer()->setAttackListToDefault();
				getPlayer()->setAlliance(ALLIANCE_FRIEND);
				{
					String tMapName;
					Vector2 tPoint;
					unsigned char tDimension = 0;
					if(mDef->loadSavePoint(tMapName,tPoint,tDimension))
					{
						resetScene(false,tMapName);
						getPlayer()->setPosition(tPoint.x,200,tPoint.y);
						mUnitManager->clampUnitToTerrain(getPlayer(),true);
					}
					else
					{
						resetScene(false,MAP_DEFAULT);
					}
					mGameStateManager->setDimension(tDimension);
				}
				mGui->setChatChannel(1);
				if(mNetworkManager->connect())mGameStateManager->setGameState(GAMESTATE_CONNECTING);
			break;
			case GAMESTATE_CONNECTING:
				mGui->update(evt);
				mSoundManager->update(evt);
				mSkyManager->update(evt);
				mNetworkManager->update(evt);
				if(mGameStateManager->getGameState()==GAMESTATE_STARTGAME)updateEverything(evt);
			break;
			case GAMESTATE_STARTGAME:
				mGui->showBackdropOverlay(false);
				mGui->showGUI(true);
				mGui->refreshBio();
				mGui->getMiniMap()->updatePortalMap();
				mGameStateManager->setGameState(GAMESTATE_INGAME);
				mGameStateManager->setGameStarted(true);
				updateEverything(evt);
			break;
			case GAMESTATE_INGAME:
			case GAMESTATE_INGAMECAMPAIGN:
				updateEverything(evt);
			break;
			case GAMESTATE_CREDITS:
				updateCredits(evt);
			break;
			case GAMESTATE_GAMEOVER:
				updateGameOver(evt);
			break;
			case GAMESTATE_CONTINUECAMPAIGN:
				mCampaignManager->continueCampaign();
			break;
			case GAMESTATE_QUITCAMPAIGN:
				mCampaignManager->closeCampaign(GAMESTATE_STARTSCREEN);
			break;
			case GAMESTATE_QUIT:
				mGameStateManager->setGameState(mGameStateManager->getTempGameState());
				contFlag = false;
			break;
		}

		return contFlag;
	}
	void updateEverything(const FrameEvent &evt)
	{
		updateCommand();
		if(mGameStateManager->isMapChange())
		{
			updateMapChange(evt);
			return;
		}
		if(!mGameStateManager->isPaused())
		{
			mUnitManager->update(evt);
			if(mUnitManager->isMapChanged())
			{
				mGui->showBackdropOverlay(true,true);
				mGui->showGUI(false);

				mGameStateManager->toggleMapChange();
				return;
			}
			mCritterManager->update(evt,getPlayer()->getObjectNode());
			mItemManager->update(evt,mCamera);
			mEffectsManager->update(evt);
		}
		if(!mGameStateManager->isPaused() || !mGameStateManager->isCampaign())mCamera->update(evt);
		mGui->update(evt);
		mSkyManager->update(evt);
		mWorld->update();
		mSoundManager->update(evt);
		mOwnershipManager->update(evt);
		if(!mGameStateManager->isCampaign())mNetworkManager->update(evt);
		if(!mGameStateManager->isPaused() && mGameStateManager->isCampaign())mCampaignManager->update(evt);

		//mDebugText = mGui->getDebugText();
		//mDebugText = mNetworkManager->getDebugText();
		//mDebugText = StringConverter::toString(getPlayer()->getPosition());
	}
	void updateCommand()
	{
		const String tCommand = mGui->popCommand();
		const String tParam = mGui->popCommandParam();

		if(tCommand==COMMAND_SETHOME)
		{
			mDef->saveHomePoint(mWorld->getWorldName(),Vector2(getPlayer()->getPosition().x,getPlayer()->getPosition().z),mGameStateManager->getDimension());
			mGui->getChatManager()->message("Home Point has been set to the current position.");
		}
		else if(tCommand==COMMAND_GOHOME)
		{
			String tMap = MAP_DEFAULT;
			Vector2 tPoint = Vector2(MAP_SPAWN);
			unsigned char tDimension = 0;
			mDef->loadHomePoint(tMap,tPoint,tDimension);

			//Same map & channel
			if(tMap == mWorld->getWorldName() && tDimension == mGameStateManager->getDimension())
			{
				getPlayer()->setPosition(tPoint.x,200,tPoint.y);
				getPlayer()->resetTarget();
				getPlayer()->stopAction();
				mUnitManager->clampUnitToTerrain(getPlayer(),true);
				mNetworkManager->sendPosition();
			}
			//Change map & dimension
			else
			{
				mUnitManager->setMapChange(true,tMap,true,Vector3(tPoint.x,0,tPoint.y));
				mGameStateManager->setDimension(tDimension);
			}
		}
		else if(tCommand==COMMAND_RESETHOME)
		{
			mDef->saveHomePoint(MAP_DEFAULT,Vector2(MAP_SPAWN),0);
			mGui->getChatManager()->message("Home Point has been reset.");
		}
		else if(tCommand==COMMAND_POSITION)
		{
			mGui->getChatManager()->message("Position is "+StringConverter::toString(getPlayer()->getPosition()));
		}
		else if(tCommand==COMMAND_EARTHQUAKE)
		{
			if(mDef->isAdmin)mNetworkManager->sendEvent(EVENT_EARTHQUAKE);
		}
		else if(tCommand==COMMAND_GOTO)
		{
			if(!mDef->isAdmin && !mDef->isMod)return;
			const vector<String>::type tPart = StringUtil::split(tParam,":",1);
			if(tPart.size()<=0)return;
			const String tMap = tPart[0];

			//Same map & channel
			if(tMap == mWorld->getWorldName())
			{
				if(tPart.size()>1)
				{
					const Vector3 tPoint = StringConverter::parseVector3(tPart[1]);
					getPlayer()->setPosition(tPoint);
					getPlayer()->resetTarget();
					getPlayer()->stopAction();
					mUnitManager->clampUnitToTerrain(getPlayer());
					mNetworkManager->sendPosition();
				}
			}
			//Change map & dimension
			else
			{
				mUnitManager->setMapChange(true,tMap,(tPart.size()>1),(tPart.size()>1?StringConverter::parseVector3(tPart[1]):Vector3::ZERO));
			}
		}
	}
	void updateMapChange(const FrameEvent &evt)
	{
		if(mGameStateManager->popMapChangeDone())
		{
			mGameStateManager->toggleMapChange();
			mGui->getMiniMap()->updatePortalMap();
			mGui->showBackdropOverlay(false);
			mGui->showGUI(mGui->getWasGUIVisible());

			mUnitManager->update(evt);
			mEffectsManager->update(evt);
			mGui->update(evt);
			if(!mGameStateManager->isPaused() || !mGameStateManager->isCampaign())mCamera->update(evt);
			mSkyManager->update(evt);
			mWorld->update();
			mSoundManager->update(evt);
			mGui->getChatManager()->message("---"+mDef->getMapName(mWorld->getWorldName())+"---");
		}
		else
		{
			const MapChangeData tMapChange = mUnitManager->getMapChange();

			resetScene(false,tMapChange.mapName);

			if(tMapChange.changePoint)
			{
				getPlayer()->setPosition(tMapChange.point.x,(tMapChange.hasVectY?tMapChange.point.y:200),tMapChange.point.z);
				mUnitManager->clampUnitToTerrain(getPlayer(),!tMapChange.hasVectY);
			}

			if(!mGameStateManager->isCampaign())
			{
				mNetworkManager->sendMapChange(tMapChange.mapName,mGameStateManager->getDimension());
				mDef->saveSavePoint(mWorld->getWorldName(),Vector2(getPlayer()->getPosition().x,getPlayer()->getPosition().z),mGameStateManager->getDimension());
			}

			mUnitManager->update(evt);
			mEffectsManager->update(evt);
			mGui->update(evt);
			if(!mGameStateManager->isPaused() || !mGameStateManager->isCampaign())mCamera->update(evt);
			mSkyManager->update(evt);
			mWorld->update();
			mSoundManager->update(evt);

			mGameStateManager->pushMapChangeDone();
		}
	}
	void updateCredits(const FrameEvent &evt)
	{
		mGui->update(evt);
		mSoundManager->update(evt);
		mCampaignManager->updateCredits(evt);
	}
	void updateGameOver(const FrameEvent &evt)
	{
		mGui->update(evt);
		mSoundManager->update(evt);
	}
	void updateLogo(const FrameEvent &evt)
	{
		mGui->update(evt);
	}
	void updateStartScreen(const FrameEvent &evt)
	{
		mNetworkManager->update(evt);
		mGui->update(evt);
		//mSoundManager->update(evt);
	}
	void updateCharScreen(const FrameEvent &evt)
	{
		mNetworkManager->update(evt);
		mGui->update(evt);
		//Re-equip items on mesh change
		if(mGui->getCharScreenManager()->popDoReequip())
			for(int i=1;i<=MAX_EQUIP;i++)
				if(getPlayer()->getEquip(i)!="")
				{
					const String tEquip = mUnitManager->unequipItem(getPlayer(),i);
					mUnitManager->equipItem(getPlayer(),tEquip,i);
				}
		mWorld->update();
		mEffectsManager->update(evt);
		mSoundManager->update(evt);
		getPlayer()->update(evt,mDef);
	}
	MagixPlayer* getPlayer()
	{
		return mUnitManager->getPlayer();
	}
	String getDebugText()
	{
		return mDebugText;
	}
	MagixGUI* getMagixGUI()
	{
		return mGui;
	}
	MagixInputManager* getInputManager()
	{
		return mGui->getInputManager();
	}
	MagixUnitManager* getUnitManager()
	{
		return mUnitManager;
	}
	MagixCamera* getCamera()
	{
		return mCamera;
	}
	MagixExternalDefinitions* getExternalDefinitions()
	{
		return mDef;
	}
	MagixNetworkManager* getNetworkManager()
	{
		return mNetworkManager;
	}
	unsigned short getCameraMode()
	{
		return mCamera->getCameraMode();
	}
	unsigned short getInputMode()
	{
		return getInputManager()->getInputMode();
	}
	MagixGameStateManager* getGameState()
	{
		return mGameStateManager;
	}
	MagixEffectsManager* getEffectsManager()
	{
		return mEffectsManager;
	}
};

#endif