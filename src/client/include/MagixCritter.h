#pragma once

#include "MagixLiving.h"
using namespace Ogre;

class MagixCritter : public MagixLiving
{
protected:
	Entity *mEnt;
	unsigned short ID;
	String type;
	short ownerIndex;
	short lastHitIndex;
	String anim;
	Real animSpeed;
	Real decisionTimer;
	Vector3 targetPosition;
	bool isDead;
	bool hasSentDeath;
	bool isDecomposing;
	bool hasRoamArea;
	unsigned char roamAreaID;
	unsigned char worldCritterID;
	bool isWorldCritter;
	Real length;
	Real height;
	bool isPet;
	bool hasAttack;
	bool isReadyToAttack;
	bool attackHasEnded;
	MagixObject *mChaseTarget;
	unsigned char attackID;
	Real stopChaseLimit;
	Real soundTimer;
	bool isDrawPoint;
public:
	MagixCritter(SceneNode *node, const unsigned short &id, const String &typeName, Entity *ent)
	{
		mObjectNode = node;
		ID = id;
		type = typeName;
		mEnt = ent;
		ownerIndex = -1;
		lastHitIndex = -1;
		decisionTimer = 0;
		targetPosition = Vector3::ZERO;
		isDead = false;
		hasSentDeath = false;
		isDecomposing = false;
		hasRoamArea = false;
		roamAreaID = 0;
		worldCritterID = 0;
		isWorldCritter = false;
		height = 0;
		length = 0;
		isPet = false;
		hasAttack = false;
		isReadyToAttack = false;
		attackHasEnded = false;
		mChaseTarget = 0;
		attackID = 0;
		stopChaseLimit = 0;
		soundTimer = 0;
		isDrawPoint = false;

		setAnimation("Idle", 0.2, true);
	}
	~MagixCritter()
	{
	}
	const OBJECT_TYPE getType();

	const unsigned short getID();

	const String getCritterType();

	void setOwnerIndex(const short &index);

	const short getOwnerIndex();

	void setImTheOwner();

	bool imTheOwner();

	void setLastHitIndex(const short &index);

	const short getLastHitIndex();

	Entity* getEnt();

	void update(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag);

	void updateSound(const FrameEvent &evt);

	void updateAnimation(const FrameEvent &evt);

	void updateAction();

	bool isAttacking();

	const String getAnim();

	bool setAnimation(const String &type, const Real &speed = 1, bool loop = false);

	void updateDecision(const FrameEvent &evt);

	void updateChase();

	void updateMovement(const FrameEvent &evt);

	void updateDeath(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag);

	void setFullColour(const ColourValue &fullColour);

	void kill();

	void setTarget(const Vector3 &position, bool clearChaseTarget = true);

	void setTarget(MagixObject *target, const unsigned char &attID = 0);

	const Vector3 getTarget();

	void setDecisionTimer(const Real &value);

	const Real getDecisionTimer();

	bool getIsDead();

	void setRoamArea(bool flag);

	bool getRoamArea();

	void setRoamAreaID(const unsigned char &iID);

	const unsigned char getRoamAreaID();

	void setWorldCritterID(const unsigned char &iID);

	bool getIsWorldCritter();

	const unsigned char getWorldCritterID();

	const Vector3 getPosition(bool headPosition = false);

	const Real getHeight();

	const Real getLength();

	void resetSize();

	void setIsPet(bool flag);

	bool getIsPet();

	void setHasAttack(bool flag);

	bool getHasAttack();

	bool popAttackHasEnded();

	bool popIsReadyToAttack();

	const unsigned char getAttackID();

	void setStopChaseLimit(const Real &y);

	MagixObject* getChaseTarget();

	void setSoundTimer(const Real &timer);

	const Real getSoundTimer();

	void setIsDrawPoint(bool flag);

	bool getIsDrawPoint();
};
