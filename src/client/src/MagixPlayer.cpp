#include "MagixPlayer.h"

MagixPlayer::MagixPlayer()
{
	controlMode = CONTROL_ABSOLUTE;
	up = 0;
	down = 0;
	left = 0;
	right = 0;
	ascend = 0;
	descend = 0;
	isLockedOn = false;
	skillSlot.clear();
	skillChangedList.clear();
	defaultAttackList.clear();
	mCurrentSkill = 0;
	mNextSkill = 0;
	autoWalk = false;
	autoRun = false;
	isReadyToPickup = false;
	pickupRangePlus = false;
	defaultControlModeIsAbsolute = true;
	isReadyToDoubleJump = false;
	index = 0;
	petName = "";
	mAutoAttackTarget = 0;
	autoAttackOnce = false;
	setPlayerTargetOnHit = false;
	hpChanged = false;
	maxHPChanged = false;
	isWounded = false;
	woundChanged = false;
	woundRecoveryTimer = 0;
	petFlags = PetFlags();
	nextAttackRange = 0;
}
MagixPlayer::~MagixPlayer()
{
	skillSlot.clear();
	defaultAttackList.clear();
	skillChangedList.clear();
}
void MagixPlayer::reset()
{
	controlMode = (defaultControlModeIsAbsolute ? CONTROL_ABSOLUTE : CONTROL_RELATIVE);
	skillSlot.clear();
	defaultAttackList.clear();
	mCurrentSkill = 0;
	stopAction();
	petName = "";
	mAutoAttackTarget = 0;
	isWounded = false;
	woundRecoveryTimer = 0;
	resetChanges();
	petFlags.reset();
	nextAttackRange = 0;
}
void MagixPlayer::resetChanges()
{
	skillChangedList.clear();
	hpChanged = false;
	maxHPChanged = false;
	woundChanged = false;
}
void MagixPlayer::updateMovement(const FrameEvent &evt, const Quaternion &camOrientation)
{
	if (isDisabled())return;
	if (isAttacking())
	{
		targetAction = 0;
		return;
	}

	if (up || down || left || right || ascend || descend)targetAction = 0;
	if (targetAction == TARGET_RUNTO || targetAction == TARGET_RUNTOPICKUP)
	{
		static_cast<MagixUnit*>(this)->updateMovement(evt);
		//Runto pickup item
		if (targetAction == TARGET_RUNTOPICKUP && mAutoTrackTarget)
		{
			const Real tDistance = mObjectNode->getPosition().squaredDistance(targetVector);
			if (tDistance <= (pickupRangePlus ? 400 : 10))
			{
				isReadyToPickup = true;
				pickupRangePlus = false;
				targetAction = 0;
			}
		}
		//Doublejump recharge
		if (distanceFromGround() <= GROUND_THRESHOLD)
		{
			isReadyToDoubleJump = false;
			hasDoubleJumped = false;
		}
		return;
	}

	if (controlMode == CONTROL_RELATIVE)
	{
		if (up)addForce(mObjectNode->getOrientation().zAxis() * 10 * evt.timeSinceLastFrame);
		if (down)addForce(mObjectNode->getOrientation().zAxis()*-5 * evt.timeSinceLastFrame);
		if (left)yaw(Degree(180 * evt.timeSinceLastFrame));
		if (right)yaw(Degree(-180 * evt.timeSinceLastFrame));
	}
	else if (controlMode == CONTROL_ABSOLUTE || controlMode == CONTROL_FIRSTPERSON)
	{
		if (up || down || left || right || ascend || descend)
		{
			Degree tYaw = Degree(camOrientation.getYaw()) + Degree(180);
			if (tYaw<Degree(0))tYaw += Degree(360);
			if (tYaw >= Degree(360))tYaw -= Degree(360);
			Vector2 tVect = Vector2(0, 0);
			if (tYaw == Degree(0) || tYaw == Degree(180))
			{
				tVect.y = (Real)(1 * (tYaw == Degree(180) ? -1 : 1));
			}
			else if (tYaw == Degree(90) || tYaw == Degree(270))
			{
				tVect.x = (Real)(1 * (tYaw == Degree(270) ? -1 : 1));
			}
			else
			{
				tVect.y = 1 * Math::Cos(tYaw);
				tVect.x = 1 * Math::Sin(tYaw);
			}

			if (up)
			{
				addForce(Vector3(tVect.x, 0, tVect.y) * 10 * evt.timeSinceLastFrame);
				setYaw(Degree(0) + tYaw);
			}
			else if (down)
			{
				if (controlMode == CONTROL_FIRSTPERSON)
				{
					addForce(Vector3(tVect.x, 0, tVect.y)*-5 * evt.timeSinceLastFrame);
					setYaw(Degree(0) + tYaw);
				}
				else
				{
					addForce(Vector3(tVect.x, 0, tVect.y)*-10 * evt.timeSinceLastFrame);
					setYaw(Degree(180) + tYaw);
				}
			}
			if (left)
			{
				addForce(Vector3(tVect.perpendicular().x, 0, tVect.perpendicular().y)*-10 * evt.timeSinceLastFrame);
				if (controlMode == CONTROL_FIRSTPERSON)
					setYaw(Degree(90) + tYaw + (up ? Degree(-45) : Degree(0)) + (down ? Degree(225) : Degree(0)));
				else setYaw(Degree(90) + tYaw + (up ? Degree(-45) : Degree(0)) + (down ? Degree(45) : Degree(0)));
			}
			else if (right)
			{
				addForce(Vector3(tVect.perpendicular().x, 0, tVect.perpendicular().y) * 10 * evt.timeSinceLastFrame);
				if (controlMode == CONTROL_FIRSTPERSON)
					setYaw(Degree(270) + tYaw + (up ? Degree(45) : Degree(0)) + (down ? Degree(-225) : Degree(0)));
				else setYaw(Degree(270) + tYaw + (up ? Degree(45) : Degree(0)) + (down ? Degree(-45) : Degree(0)));
			}
		}
	}

	/*if (jump && distanceFromGround()<(MAX_JUMPHEIGHT*hasDoubleJumped ? 2 : 1) && mForce.y >= 0)
		addForce(Vector3(0, JUMP_GRAVITY*evt.timeSinceLastFrame + (distanceFromGround() <= GROUND_THRESHOLD ? JUMP_GRAVITY : 0), 0));*/
	if (jump && distanceFromGround()==0 && mForce.y >= 0)
		addForce(Vector3(0, JUMP_ACCEL, 0));
	//Doublejump recharge
	if (distanceFromGround() <= GROUND_THRESHOLD)
	{
		isReadyToDoubleJump = false;
		hasDoubleJumped = false;
	}

	if (ascend && antiGravity)addForce(Vector3(0, 5 * evt.timeSinceLastFrame, 0));
	else if (descend && antiGravity)addForce(Vector3(0, -5 * evt.timeSinceLastFrame, 0));
}
void MagixPlayer::updateAutoAttack(MagixExternalDefinitions *def)
{
	if (!mAutoAttackTarget || (up || down || left || right || ascend || descend))return;
	if (getPosition().squaredDistance(mAutoAttackTarget->getPosition())<(AUTOTARGET_RANGE_SQUARED*0.25 + nextAttackRange*nextAttackRange))
	{
		if (!mNextSkill)doAttack(def);
		else doSkill(mNextSkill);
		nextAttackRange = 0;
	}
	else if (targetAction == 0 || mAutoAttackTarget->getPosition() != targetVector)setTarget(mAutoAttackTarget->getPosition());
}
bool MagixPlayer::updateAttack(const FrameEvent &evt, MagixExternalDefinitions *def)
{
	if (isAttacking())
	{
		if (mBodyEnt->getAnimationState(animBase)->hasEnded())
		{
			attackTimeout = 0.25;
			hasStoppedAttack = true;
		}
		else if (getControlMode() != CONTROL_FIRSTPERSON)lookDirection = mObjectNode->getOrientation()*Quaternion(0, 0, 1, 0);
	}
	if (!isAttacking() && nextAttackName != "")
	{
		attackName = nextAttackName;
		nextAttackName = "";
		attackTimeout = 0;
		const String tAnim = def->getAttackAnim(attackName);
		changeAnimBase(tAnim, 1.5, true, false, false);
		hasNewAttack = true;

		if (mNextSkill && mNextSkill->name == attackName)
		{
			decrementSkill(mNextSkill);
			mNextSkill = 0;
		}
	}
	if (attackTimeout>0)
	{
		attackTimeout -= evt.timeSinceLastFrame;
		if (attackTimeout<0)attackTimeout = 0;
		if (attackTimeout == 0)
		{
			attackName = "";
			attackCounter = 0;
		}
	}
	return isAttacking();
}
void MagixPlayer::doSkill(Skill *skill, bool waitTillInRange)
{
	if (!waitTillInRange)nextAttackName = skill->name;
	mNextSkill = skill;
}
void MagixPlayer::setAttackRange(const unsigned short &range)
{
	nextAttackRange = range;
}
void MagixPlayer::updateWound(const FrameEvent &evt)
{
	if (!isWounded)return;
	if (hp <= 0)return;

	//Recover from wound
	if (woundRecoveryTimer>0)
	{
		Real rate = 1;
		if (isSitting() || isLaying() || isSideLaying())rate = 25;
		if (isRolledOver() || isPlopped() || isLeaning() || isCurled())rate = 50;
		if (isInWater)rate *= 2;
		woundRecoveryTimer -= rate*evt.timeSinceLastFrame;
	}
	if (woundRecoveryTimer>0)return;
	woundRecoveryTimer += 10;
	//Recovered
	if (addHPRatio(-0.001))setWounded(false);
}
#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
void MagixPlayer::update(const FrameEvent &evt, MagixExternalDefinitions *def, const Quaternion &camOrientation)
{
	if (!unitID)return;

	
//	DBOUT("1 MagixPlayer::update: " << evt.timeSinceLastFrame << std::endl);

	updatePhysics(evt);
	updateMovement(evt, camOrientation);
	updateAutoAttack(def);
	updateAction(evt, def);
	updateAutoTrack();
	updateAnimation(evt, def);
	updateFaceDirection(evt);
	updateNameTag(evt);
	updateUserTag(evt);
	updateChatBubble(evt);
	updateWound(evt);
}
const OBJECT_TYPE MagixPlayer::getType()
{
	return OBJECT_PLAYER;
}
void MagixPlayer::toggleControlMode()
{
	switch (controlMode)
	{
	case CONTROL_RELATIVE:
		controlMode = CONTROL_ABSOLUTE;
		defaultControlModeIsAbsolute = true;
		break;
	case CONTROL_ABSOLUTE:
		controlMode = CONTROL_RELATIVE;
		defaultControlModeIsAbsolute = false;
		break;
	}
}
void MagixPlayer::setTarget(const Vector3 &dest, const unsigned char &action, bool pickupPlus)
{
	static_cast<MagixUnit*>(this)->setTarget(dest.x, dest.y, dest.z, action);
	pickupRangePlus = pickupPlus;
}
void MagixPlayer::setControlMode(const unsigned short &mode)
{
	controlMode = mode;
	if (controlMode == CONTROL_ABSOLUTE)defaultControlModeIsAbsolute = true;
	else if (controlMode == CONTROL_RELATIVE)defaultControlModeIsAbsolute = false;
}
unsigned short MagixPlayer::getControlMode()
{
	return controlMode;
}
void MagixPlayer::stopAction()
{
	up = 0;
	down = 0;
	left = 0;
	right = 0;
	ascend = 0;
	descend = 0;
	isLockedOn = false;
	isWalking = autoWalk;
	isCrouching = false;
	jump = false;
	antiGravity = false;
	isSwimming = false;
	targetAction = 0;
	stopAttack();
	autoRun = false;
	isReadyToPickup = false;
	pickupRangePlus = false;
	isReadyToDoubleJump = false;
	mAutoAttackTarget = 0;
}
void MagixPlayer::lockOn(MagixObject *target)
{
	setAutoTrackObject(target);
	if (target)
	{
		isLockedOn = true;
		setPlayerTargetOnHit = false;
	}
	else
	{
		if (isLockedOn)
		{
			isLockedOn = false;
			setPlayerTargetOnHit = true;
		}
		targetAction = 0;

	}
}
bool MagixPlayer::getIsLockedOn()
{
	return isLockedOn;
}
void MagixPlayer::resetSkills()
{
	skillSlot.clear();
	mCurrentSkill = 0;
}
void MagixPlayer::setSkills(const vector<Skill>::type &skills)
{
	skillSlot.clear();
	skillSlot = skills;
	mCurrentSkill = 0;
}
const vector<Skill>::type& MagixPlayer::getSkills()
{
	return skillSlot;
}
void MagixPlayer::addSkill(const String &name, const unsigned short &stock)
{
	for (vector<Skill>::type::iterator it = skillSlot.begin(); it != skillSlot.end(); it++)
	{
		Skill *skill = &*it;
		if (skill->name == name)
		{
			skill->stock += stock;
			if (skill->stock>MAX_SKILLSTOCK)skill->stock = MAX_SKILLSTOCK;
			skillChangedList.push_back(Skill(name, skill->stock));
			return;
		}
	}
	skillSlot.push_back(Skill(name, (stock>MAX_SKILLSTOCK ? MAX_SKILLSTOCK : stock)));
	skillChangedList.push_back(Skill(name, (stock>MAX_SKILLSTOCK ? MAX_SKILLSTOCK : stock)));
}
void MagixPlayer::decrementSkill(Skill *skill)
{
	if (!skill)return;
	skill->stock -= 1;
	skillChangedList.push_back(Skill(skill->name, skill->stock));
	if (skill->stock == 0)
	{
		if (mCurrentSkill == skill)setCurrentSkill(0);
		for (vector<Skill>::type::iterator it = skillSlot.begin(); it != skillSlot.end(); it++)
		{
			Skill *tSkill = &*it;
			if (tSkill == skill)
			{
				skillSlot.erase(it);
				return;
			}
		}
	}
}
void MagixPlayer::clearDefaultAttackList()
{
	defaultAttackList.clear();
}
void MagixPlayer::pushDefaultAttackList(const String &attackName)
{
	defaultAttackList.push_back(attackName);
}
void MagixPlayer::setAttackListToDefault()
{
	attackList.clear();
	attackList = defaultAttackList;
}
void MagixPlayer::setCurrentSkill(Skill *skill)
{
	mCurrentSkill = 0;
	if (!skill)
	{
		setAttackListToDefault();
		return;
	}
	for (vector<Skill>::type::iterator it = skillSlot.begin(); it != skillSlot.end(); it++)
	{
		Skill *tSkill = &*it;
		if (tSkill == skill)
		{
			mCurrentSkill = skill;
			//clearAttackList();
			//pushAttackList(skill->name);
			return;
		}
	}
}
Skill* MagixPlayer::getNextSkill()
{
	bool nextOneIsIt = false;
	Skill *tSkill = 0;
	for (vector<Skill>::type::iterator it = skillSlot.begin(); it != skillSlot.end(); it++)
	{
		Skill *skill = &*it;
		if (nextOneIsIt)
		{
			tSkill = skill;
			break;
		}
		if (!mCurrentSkill)return skill;
		else if (mCurrentSkill == skill)nextOneIsIt = true;
	}
	if (!tSkill && skillSlot.size()>0)tSkill = &skillSlot[0];
	return tSkill;
}
Skill* MagixPlayer::getSkill(const String &name)
{
	for (vector<Skill>::type::iterator it = skillSlot.begin(); it != skillSlot.end(); it++)
	{
		Skill *skill = &*it;
		if (skill->name == name)
		{
			return skill;
		}
	}
	return 0;
}
Skill* MagixPlayer::getCurrentSkill()
{
	return mCurrentSkill;
}
bool MagixPlayer::isSkillMode()
{
	return (mCurrentSkill != 0);
}
void MagixPlayer::toggleAutoWalk()
{
	autoWalk = !autoWalk;
	isWalking = !isWalking;
}
bool MagixPlayer::getAutoWalk()
{
	return autoWalk;
}
void MagixPlayer::toggleAutoRun()
{
	autoRun = !autoRun;
	up = autoRun;
}
bool MagixPlayer::getAutoRun()
{
	return autoRun;
}
bool MagixPlayer::popIsReadyToPickup()
{
	const bool tFlag = isReadyToPickup;
	isReadyToPickup = false;
	return tFlag;
}
void MagixPlayer::setIndex(const unsigned short &value)
{
	index = value;
}
const unsigned short MagixPlayer::getIndex()
{
	return index;
}
void MagixPlayer::setPet(const String &type, MagixExternalDefinitions *def)
{
	petName = type;
	if (def && type != "")
	{
		petFlags.hasHeal = (def->getCritterRandomSpecificAttack(type, false) != 0);
	}
}
bool MagixPlayer::hasPet()
{
	return (petName != "");
}
const String MagixPlayer::getPet()
{
	return petName;
}
void MagixPlayer::setAutoAttack(MagixLiving *target, bool attackOnce)
{
	mAutoAttackTarget = target;
	autoAttackOnce = attackOnce;
}
MagixLiving* MagixPlayer::getAutoAttackTarget()
{
	return mAutoAttackTarget;
}
bool MagixPlayer::getAutoAttackOnce()
{
	return autoAttackOnce;
}
const vector<const Skill>::type MagixPlayer::popSkillChangedList()
{
	const vector<const Skill>::type tList = skillChangedList;
	skillChangedList.clear();
	return tList;
}
bool MagixPlayer::popHPChanged()
{
	const bool tFlag = hpChanged;
	hpChanged = false;
	return tFlag;
}
bool MagixPlayer::popMaxHPChanged()
{
	const bool tFlag = maxHPChanged;
	maxHPChanged = false;
	return tFlag;
}
void MagixPlayer::setHP(const short &iMaxHP, const Real &ratio)
{
	maxHP = iMaxHP;
	hp = (short)(ratio*maxHP);
	maxHPChanged = true;
}
void MagixPlayer::setHPRatio(const Real &ratio)
{
	hp = (short)(ratio*maxHP);
	hpChanged = true;
}
bool MagixPlayer::addHPRatio(const Real &ratio)
{
	if (invulnerable && ratio<0)return false;
	hpChanged = true;
	hp += (short)(ratio*maxHP);
	if (hp>maxHP)hp = maxHP;
	if (hp<0)hp = 0;
	return (hp == 0);
}
bool MagixPlayer::addHP(const short &value)
{
	if (invulnerable && value<0)return false;
	hpChanged = true;
	hp += value;
	if (hp>maxHP)hp = maxHP;
	if (hp<0)hp = 0;
	return (hp == 0);
}
void MagixPlayer::setWounded(bool flag, bool instant)
{
	isWounded = flag;
	if (!instant)woundChanged = true;
}
bool MagixPlayer::getIsWounded()
{
	return isWounded;
}
bool MagixPlayer::popWoundedChanged()
{
	const bool tFlag = woundChanged;
	woundChanged = false;
	return tFlag;
}
void MagixPlayer::setPickupRangePlus(bool flag)
{
	pickupRangePlus = flag;
}
PetFlags* MagixPlayer::getPetFlags()
{
	return &petFlags;
}