#pragma once

#define MAX_SKILLSTOCK 200
#define AUTOTARGET_RANGE_SQUARED 6400

#include "MagixUnit.h"

using namespace Ogre;

struct Skill
{
	String name;
	unsigned char stock;
	Skill(const String &skillName, const unsigned char &skillStock)
	{
		name = skillName;
		stock = skillStock;
	}
};
struct PetFlags
{
	bool stay;
	MagixLiving *attackTarget;
	bool shrink;
	bool evolve;
	bool hasHeal;
	PetFlags()
	{
		stay = false;
		attackTarget = 0;
		shrink = false;
		evolve = false;
		hasHeal = false;
	}
	void reset()
	{
		attackTarget = 0;
	}
};

class MagixPlayer : public MagixUnit
{
protected:
	unsigned short controlMode;
	bool isLockedOn;
	vector<Skill>::type skillSlot;
	vector<const Skill>::type skillChangedList;
	vector<const String>::type defaultAttackList;
	Skill *mCurrentSkill;
	Skill *mNextSkill;
	bool autoWalk;
	bool autoRun;
	bool isReadyToPickup;
	bool pickupRangePlus;
	unsigned short index;
	String petName;
	MagixLiving *mAutoAttackTarget;
	bool autoAttackOnce;
	bool hpChanged;
	bool maxHPChanged;
	bool isWounded;
	bool woundChanged;
	Real woundRecoveryTimer;
	PetFlags petFlags;
	unsigned short nextAttackRange;
	
	Real lastUpdate;
public:
	bool up;
	bool down;
	bool left;
	bool right;
	bool ascend;
	bool descend;
	bool defaultControlModeIsAbsolute;
	bool isReadyToDoubleJump;
	bool setPlayerTargetOnHit;

	MagixPlayer();
	~MagixPlayer();
	void reset();
	void resetChanges();
	void updateMovement(const FrameEvent &evt, const Quaternion &camOrientation = Quaternion::ZERO);
	void updateAutoAttack(MagixExternalDefinitions *def);
	bool updateAttack(const FrameEvent &evt, MagixExternalDefinitions *def);
	void doSkill(Skill *skill, bool waitTillInRange = false);
	void setAttackRange(const unsigned short &range);
	void updateWound(const FrameEvent &evt);
	void update(const FrameEvent &evt, MagixExternalDefinitions *def, const Quaternion &camOrientation = Quaternion::ZERO);
	const OBJECT_TYPE getType();
	void toggleControlMode();
	void setTarget(const Vector3 &dest, const unsigned char &action = TARGET_RUNTO, bool pickupPlus = false);
	void setControlMode(const unsigned short &mode);
	unsigned short getControlMode();
	void stopAction();
	void lockOn(MagixObject *target);
	bool getIsLockedOn();
	void resetSkills();
	void setSkills(const vector<Skill>::type &skills);
	const vector<Skill>::type& getSkills();
	void addSkill(const String &name, const unsigned short &stock);
	void decrementSkill(Skill *skill);
	void clearDefaultAttackList();
	void pushDefaultAttackList(const String &attackName);
	void setAttackListToDefault();
	void setCurrentSkill(Skill *skill);
	Skill* getNextSkill();
	Skill* getSkill(const String &name);
	Skill* getCurrentSkill();
	bool isSkillMode();
	void toggleAutoWalk();
	bool getAutoWalk();
	void toggleAutoRun();
	bool getAutoRun();
	bool popIsReadyToPickup();
	void setIndex(const unsigned short &value);
	const unsigned short getIndex();
	void setPet(const String &type, MagixExternalDefinitions *def = 0);
	bool hasPet();
	const String getPet();
	void setAutoAttack(MagixLiving *target, bool attackOnce = false);
	MagixLiving* getAutoAttackTarget();
	bool getAutoAttackOnce();
	const vector<const Skill>::type popSkillChangedList();
	bool popHPChanged();
	bool popMaxHPChanged();
	void setHP(const short &iMaxHP, const Real &ratio = 1);
	void setHPRatio(const Real &ratio);
	bool addHPRatio(const Real &ratio);
	bool addHP(const short &value);
	void setWounded(bool flag, bool instant = false);
	bool getIsWounded();
	bool popWoundedChanged();
	void setPickupRangePlus(bool flag);
	PetFlags* getPetFlags();
};
