#pragma once

#include "MagixUnit.h"
#include "MagixGameStateManager.h"
#include "MagixGUI.h"
#include "MagixUnitManager.h"
#include "MagixWorld.h"
#include "MagixSkyManager.h"
#include "MagixCamera.h"
#include "MagixSoundManager.h"
#include "MagixCollisionManager.h"
#include "MagixEffectsManager.h"
#include "MagixConst.h"

struct ScriptedUnit
{
	MagixUnit *mUnit;
	String script;
	Real attackFrequency;
	MagixUnit *chaseUnit;
	Real timer;
	CampaignEventList eventList;
	vector<const String>::type nextEvent;
	vector<vector<const String>::type>::type eventConditionList;
	ScriptedUnit()
	{
		mUnit = 0;
		script = "";
		attackFrequency = 0;
		chaseUnit = 0;
		timer = 0;
		eventList.clear();
		nextEvent.clear();
		eventConditionList.clear();
	}
};

class MagixCampaignManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixGUI *mGui;
	MagixUnitManager *mUnitManager;
	MagixWorld *mWorld;
	MagixSkyManager *mSkyManager;
	MagixCamera *mCamera;
	MagixSoundManager *mSoundManager;
	MagixCollisionManager *mCollisionManager;
	MagixEffectsManager *mEffectsManager;
	unsigned short progress;
	unsigned short progressCheckpoint;
	Real cinematicTimer;
	bool randomSpawn;
	Real randomSpawnTimer;
	vector<MagixUnit*>::type mSpawn;
	vector<ScriptedUnit>::type mScriptedUnits;
	vector<String>::type credits;
	Real creditsTimeout;
	unsigned short creditsCounter;
	Real gameOverTimeout;
	vector<Skill>::type skillsCheckpoint;
	String campaignFilename;
	CampaignEventList eventList;
	vector<const String>::type nextEvent;
	vector<vector<const String>::type>::type eventConditionList;
	bool ignoreGameOver;
public:
	MagixCampaignManager()
	{
		mDef = 0;
		mGameStateManager = 0;
		mGui = 0;
		mUnitManager = 0;
		mWorld = 0;
		mSkyManager = 0;
		mCamera = 0;
		mSoundManager = 0;
		mCollisionManager = 0;
		mEffectsManager = 0;
		progress = 0;
		progressCheckpoint = 0;
		cinematicTimer = 0;
		randomSpawn = false;
		randomSpawnTimer = 0;
		mSpawn.clear();
		mScriptedUnits.clear();
		credits.clear();
		creditsTimeout = 0;
		creditsCounter = 0;
		gameOverTimeout = 0;
		skillsCheckpoint.clear();
		campaignFilename = "";
		eventList.clear();
		nextEvent.clear();
		eventConditionList.clear();
		ignoreGameOver = false;
	}

	~MagixCampaignManager()
	{
		mScriptedUnits.clear();
	}

	void initialize(MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixGUI *gui, 
		MagixUnitManager *unitMgr, MagixWorld *world, MagixSkyManager *skyMgr, MagixCamera *camera,
		MagixSoundManager *soundMgr, MagixCollisionManager *collisionMgr, MagixEffectsManager *fxMgr);

	void loadCampaign();

	void update(const FrameEvent &evt);

	void updateScriptedUnits(const FrameEvent &evt);

	void continueCampaign();

	bool checkGameOver(const FrameEvent &evt);

	void doRandomSpawn();

	void createSpawns(unsigned short number);

	void updateSpawns();

	void killSpawns();

	bool unitIsInRegion(MagixUnit *unit, const Vector2 &center, const Real &range);

	void setCheckpoint();

	void toggleCinematic(bool skipTransition = false);

	void closeCampaign(unsigned short restartState = GAMESTATE_INITIALIZE, const String &creditsName = "");

	void updateCredits(const FrameEvent &evt);

	bool compare(String first, String second);

	void processEvent(const CampaignEvent &cEvent, bool isInitialize = false, ScriptedUnit *sUnit = 0);

	void registerEventConditions(const CampaignEvent &cEvent, ScriptedUnit *sUnit = 0);

	void checkEventCondition(ScriptedUnit *sUnit = 0);

	const Real parseReal(const String &data);

	const unsigned short parseProgress(const String &data);

	MagixUnit* parseUnit(const String &data, ScriptedUnit *sUnit = 0);

	const Vector2 parseVector2(const String &data, ScriptedUnit *sUnit = 0);

	const Vector3 parseVector3(const String &data, ScriptedUnit *sUnit = 0, bool isVector2 = false);

	const Quaternion parseQuaternion(const String &data);

	const unsigned short parseAction(const String &data);

	const unsigned short parseAlliance(const String &data);

	const short parseItemSlot(const String &data, MagixUnit *unit = 0);
};
