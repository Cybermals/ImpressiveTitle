#pragma once

#include "MagixCritter.h"
#include "MagixCollisionManager.h"
#include "MagixEffectsManager.h"
#include "MagixSoundManager.h"
#include "MagixWorld.h"
#include "MagixGameStateManager.h"

class MagixCritterManager
{
protected:
	SceneManager *mSceneMgr;
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixEffectsManager *mEffectsManager;
	MagixCollisionManager *mCollisionManager;
	MagixSoundManager *mSoundManager;
	MagixWorld *mWorld;
	MagixCamera *mCamera;
	RaySceneQuery *mRayQuery;
	vector<MagixCritter*>::type critterList;
	vector<MagixCritter*>::type killedQueue;
	vector<MagixCritter*>::type deadQueue;
	vector<MagixCritter*>::type decisionQueue;
	vector<MagixCritter*>::type stationaryQueue;
	vector<const HitInfo>::type hitQueue;
	MagixLiving *mPlayerTarget;
	vector<MagixCritter*>::type myCritters;
	vector<const unsigned short>::type sentID;
	unsigned short critterCount;
	String weatherEffectRequest;
public:
	MagixCritterManager()
	{
		mSceneMgr = 0;
		mDef = 0;
		mGameStateManager = 0;
		mEffectsManager = 0;
		mCollisionManager = 0;
		mSoundManager = 0;
		mWorld = 0;
		mCamera = 0;
		mRayQuery = 0;
		critterList.clear();
		killedQueue.clear();
		deadQueue.clear();
		decisionQueue.clear();
		stationaryQueue.clear();
		hitQueue.clear();
		mPlayerTarget = 0;
		myCritters.clear();
		sentID.clear();
		critterCount = 0;
		weatherEffectRequest = "";
	}

	~MagixCritterManager()
	{
		reset();
		if(mRayQuery)delete mRayQuery;
	}

	void initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixEffectsManager *effectsMgr,
		MagixCollisionManager *collMgr, MagixSoundManager *soundMgr, MagixWorld *world, MagixCamera *camera);

	void reset();

	void update(const FrameEvent &evt, SceneNode *playerNode = 0);

	bool isInRange(MagixCritter *critter);

	void boundCritterToMap(MagixCritter *critter);

	void updateCollision(MagixCritter *critter, SceneNode *playerNode, bool inRange = false);

	void hitCritter(MagixCritter *critter, const Real &damage, const Vector3 &force, const OwnerToken &token = 0, bool inRange = false);

	void hitCritter(const unsigned short &iID, const Real &damage, const Vector3 &force, const OwnerToken &token);

	void updateGroundHeight(MagixCritter *critter, bool checkWaterCollision = false, bool checkCollBoxCollision = false);

	MagixCritter* createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const short &owner = -1);

	MagixCritter* createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const unsigned char &worldID, const short &owner = -1);

	void deleteCritter(MagixCritter *tCritter, const vector<MagixCritter*>::type::iterator &it);

	void deleteCritter(const unsigned short &iID);

	void deleteAllCritters();

	MagixCritter* getByObjectNode(SceneNode *node);

	MagixCritter* getByID(const unsigned short &iID);

	const unsigned short getNumCritters(bool includePets = false);

	void decrementCritterCount();

	const unsigned short getNextEmptyID(const unsigned short &start);

	const vector<MagixCritter*>::type getCritterList();

	const vector<MagixCritter*>::type popKilledQueue();

	const vector<MagixCritter*>::type popDeadQueue();

	const vector<const HitInfo>::type popHitQueue();

	void pushHitQueue(const HitInfo &info);

	const vector<MagixCritter*>::type popDecisionQueue();

	const vector<MagixCritter*>::type popStationaryQueue();

	const vector<MagixCritter*>::type getMyCritters();

	void pushMyCritters(MagixCritter *critter);

	void removeFromMyCritters(MagixCritter *critter);

	MagixLiving* popPlayerTarget();

	void removeChaseTarget(MagixObject *target);

	void pushSentID(const unsigned short &iID);

	void popSentID(const unsigned short &iID);

	void setIsPet(MagixCritter *critter);

	const String popWeatherEffectRequest();
};
