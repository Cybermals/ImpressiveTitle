#pragma once

#include "MagixGameStateManager.h"
#include "MagixItem.h"
#include "MagixItemManager.h"
#include "MagixCritterManager.h"
#include "MagixPlayer.h"
#include "MagixIndexedUnit.h"
#include "MagixWorld.h"

#define EARSHOT_SQUARED 4000000

using namespace Ogre;

struct MapChangeData
{
	bool changeMap;
	String mapName;
	bool changePoint;
	Vector3 point;
	bool hasVectY;

	MapChangeData()
	{
		changeMap = false;
		mapName = "";
		changePoint = false;
		point = Vector3::ZERO;
		hasVectY = false;
	}
};

struct TameData
{
	bool hasData;
	bool success;
	String message;
	unsigned short id;
	TameData(bool hD=false,bool s=false,const String &m="",const unsigned short &i=0)
	{
		hasData = hD;
		success = s;
		message = m;
		id = i;
	}
};


class MagixUnitManager
{
protected:
	SceneManager *mSceneMgr;
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixPlayer *mPlayer;
	MagixIndexedUnit *mLastUnit;
	MagixWorld *mWorld;
	MagixEffectsManager *mEffectsManager;
	MagixCollisionManager *mCollisionManager;
	MagixSoundManager *mSoundManager;
	MagixItemManager *mItemManager;
	MagixCritterManager *mCritterManager;
	MagixCamera *mCamera;
	RaySceneQuery *mRayQuery;
	MovableTextOverlayAttributes *mNameTagAttr;
	MovableTextOverlayAttributes *mUserTagAttr;
	MovableTextOverlayAttributes *mChatBubbleAttr;
	MagixLiving *mPlayerTarget;
	bool playerTargetChanged;
	bool shouldNameTagsBeVisible;
	MapChangeData mapChange;
	vector<const std::pair<String,Vector2>>::type itemDropQueue;
	String pickupText;
	bool playerHasNewAttack;
	vector<const std::pair<OwnerToken,String>>::type partyMembers;
	bool partyChanged;
	OwnerToken partyInviter;
	vector<const HitInfo>::type hitQueue;
	TameData tameFlag;

public:
	MagixUnitManager();
	~MagixUnitManager();
	void initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixWorld *world, MagixEffectsManager *effectsMgr, 
		MagixCollisionManager *collisionMgr, MagixSoundManager *soundMgr, MagixItemManager *itemMgr, MagixCritterManager *critterMgr, MagixCamera *camera);
	void reset(bool destroyPlayer = false, bool resetPosition = true);
	void resetPlayerPosition();
	void reclampAllUnitsToTerrain(bool clampToTerrain = false);
	void update(const FrameEvent &evt);
	void updateUnit(MagixUnit *unit, const FrameEvent &evt, bool isPlayer);
	bool isInRange(MagixUnit *unit);
	void hitUnit(MagixUnit *unit, const Real &damage, const Vector3 &force, bool inRange = false);
	void hitUnit(const OwnerToken &iID, const Real &damage, const Vector3 &force);
	void updateWallCollisions(MagixUnit *unit);
	void boundUnitToMap(MagixUnit *unit);
	void updatePortalCollisions(MagixUnit *unit);
	void updateGateCollisions(MagixUnit *unit);
	void updateCollBoxCollisions(MagixUnit *unit, const FrameEvent &evt);
	void updateWaterCollisions(MagixUnit *unit);
	void updateMapEffects(MagixUnit *unit);
	void updateSounds(MagixUnit *unit);
	bool isMapChanged();
	const MapChangeData getMapChange();
	void setMapChange(bool changeMap, const String &mapName, bool changePoint = false, const Vector3 &point = Vector3::ZERO, bool hasVectY = false);
	const Quaternion doAutoTarget(MagixUnit *unit, bool targetClosest = false);
	bool doLockOn();
	void clampUnitToTerrain(MagixAnimated *unit, bool clampToGround = false);
	void clampUnitToCeiling(MagixUnit *unit);
	const Real getGroundHeight(const Real &x, const Real &z, bool checkWaterCollision = false, bool checkCollBoxCollision = false);
	MagixPlayer* getPlayer();
	MagixIndexedUnit* createUnit(const unsigned short iIndex, const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh);
	MagixIndexedUnit* getUnitByIndex(const unsigned short &iIndex);
	void deleteUnit(MagixIndexedUnit *target);
	void deleteAllUnits();
	void createNameTag(MagixUnit *target, const String &name);
	void createUserTag(MagixUnit *target, const String &name);
	void createChatBubble(MagixUnit *target, const String &caption);
	void updateNameTag(/*RectLayoutManager &m, */MovableTextOverlay *p);
	void updateChatBubble(/*RectLayoutManager &m, */MovableTextOverlay *p);
	MagixUnit* getByObjectNode(SceneNode *objectNode);
	MagixUnit* getByName(const String &name);
	MagixUnit* getByUser(String name);
	bool popPlayerTargetChanged();
	MagixLiving* getPlayerTarget();
	void setPlayerTarget(MagixLiving *unit);
	void showNameTags(bool flag);
	void setShouldNameTagsBeVisible(bool flag);
	bool getShouldNameTagsBeVisible();
	void destroyUnit(MagixUnit *unit);
	void createPlayerUnit(const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh);
	const vector<MagixIndexedUnit*>::type getUnitList();
	ParticleSystem* createParticleOnUnit(MagixUnit *unit, const String &particleName, const Real &duration, bool unresettable = false);
	ParticleSystem* createParticleOnUnitBone(MagixUnit *unit, const String &tBone, const String &particleName, const Real &duration, bool unresettable = false);
	void createAdminAura(MagixUnit *unit, const ColourValue &colour);
	void destroyParticleOnUnit(MagixUnit *unit, const String &particleName);
	bool equipItem(MagixUnit *unit, const String &meshName, const short &slot);
	const short getNextEmptyItemSlot(MagixUnit *unit);
	const short getNextFilledItemSlot(MagixUnit *unit);
	const String unequipItem(MagixUnit *unit, const unsigned short &iID);
	void createItem(const unsigned short &iID, const String &meshName, const Vector2 &position);
	void createItem(const unsigned short &iID, const String &meshName, const Real &tX, const Real &tZ);
	void deleteItem(const unsigned short &iID);
	void deleteItem(MagixItem *item);
	MagixCritter* createCritter(const unsigned short &iID, const unsigned char &worldID, const Vector3 &position, const short &owner = -1);
	MagixCritter* createCritter(const unsigned short &iID, const String &type, const Vector3 &position, const short &owner = -1);
	void deleteCritter(const unsigned short &iID);
	const vector<const std::pair<String, Vector2>>::type popItemDropQueue();
	const String popPickupText();
	void killAndRewardCritter(MagixCritter *critter, bool imTheKiller = true);
	void rewardCritter(MagixCritter *critter);
	bool popPlayerHasNewAttack();
	void addPartyMember(const OwnerToken &token, const String &name);
	bool removePartyMember(const String &name);
	void clearPartyMembers();
	bool isPartyFull();
	const vector<const std::pair<OwnerToken, String>>::type getPartyMembers();
	bool isPartyMember(MagixUnit *unit);
	bool popPartyChanged();
	bool hasParty();
	void setPartyInviter(const OwnerToken &token);
	const OwnerToken getPartyInviter();
	const std::pair<OwnerToken, String> getPartyMember(const OwnerToken &token);
	const vector<const HitInfo>::type popHitQueue();
	void pushHitQueue(const HitInfo &info);
	const TameData popTameFlag();
};
