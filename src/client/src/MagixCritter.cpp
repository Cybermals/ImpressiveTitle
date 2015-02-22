#include "MagixCritter.h"

#include "Ogre.h"
using namespace Ogre;

const OBJECT_TYPE MagixCritter::getType()
{
	return OBJECT_CRITTER;
}
const unsigned short MagixCritter::getID()
{
	return ID;
}
const String MagixCritter::getCritterType()
{
	return type;
}
void MagixCritter::setOwnerIndex(const short &index)
{
	ownerIndex = index;
}
const short MagixCritter::getOwnerIndex()
{
	return ownerIndex;
}
void MagixCritter::setImTheOwner()
{
	ownerIndex = 0;
}
bool MagixCritter::imTheOwner()
{
	return (ownerIndex == 0);
}
void MagixCritter::setLastHitIndex(const short &index)
{
	lastHitIndex = index;
}
const short MagixCritter::getLastHitIndex()
{
	return lastHitIndex;
}
Entity* MagixCritter::getEnt()
{
	return mEnt;
}
void MagixCritter::update(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag)
{
	if (!isDead && (hp>0 || isPet))
	{
		if (imTheOwner())updateDecision(evt);
		updateChase();
		updateMovement(evt);
		if (isPet)
		{
			if (hp <= 0)updateDeath(evt, killedFlag, decomposedFlag);
			else if (hasSentDeath)hasSentDeath = false;
		}
	}
	else updateDeath(evt, killedFlag, decomposedFlag);
	updatePhysics(evt);
	updateAction();
	updateAnimation(evt);
	updateFaceDirection(evt);
	updateSound(evt);
	if (height == 0)height = mEnt->getWorldBoundingBox().getMaximum().y - mEnt->getWorldBoundingBox().getMinimum().y;
	if (length == 0)length = mEnt->getWorldBoundingBox().getMaximum().z - mEnt->getWorldBoundingBox().getMinimum().z;
}
void MagixCritter::updateSound(const FrameEvent &evt)
{
	if (soundTimer>0)soundTimer -= evt.timeSinceLastFrame;
	if (soundTimer<0)soundTimer = 0;
}
void MagixCritter::updateAnimation(const FrameEvent &evt)
{
	if (!mEnt->isVisible())return;
	if (anim != "")mEnt->getAnimationState(anim)->addTime(evt.timeSinceLastFrame*animSpeed);
}
void MagixCritter::updateAction()
{
	if (isDead){ setAnimation("Die", 1, false); return; }
	if (hasAttack && isAttacking())
	{
		if (mEnt->getAnimationState(anim)->hasEnded())attackHasEnded = true;
		else return;
	}
	if (mForce.x != 0 || mForce.z != 0){ setAnimation("Run", 1, true); return; }
	if (anim == "" || anim == "Run" || mEnt->getAnimationState(anim)->hasEnded())
		setAnimation("Idle", 0.2, true);
}
bool MagixCritter::isAttacking()
{
	return (StringUtil::startsWith(anim, "Attack", false));
}
const String MagixCritter::getAnim()
{
	return anim;
}
bool MagixCritter::setAnimation(const String &type, const Real &speed, bool loop)
{
	if (anim == type)return false;
	if (!mEnt->getSkeleton()->hasAnimation(type))return false;
	if (anim != "")mEnt->getAnimationState(anim)->setEnabled(false);
	anim = type;
	animSpeed = speed;
	mEnt->getAnimationState(anim)->setEnabled(true);
	mEnt->getAnimationState(anim)->setLoop(loop);
	mEnt->getAnimationState(anim)->setTimePosition(0);
	return true;
}
void MagixCritter::updateDecision(const FrameEvent &evt)
{
	if (decisionTimer>0)decisionTimer -= evt.timeSinceLastFrame;
	if (decisionTimer<0)decisionTimer = 0;
	if (decisionTimer>0)return;
}
void MagixCritter::updateChase()
{
	if (!mChaseTarget)return;
	if (stopChaseLimit>0 && mChaseTarget->getPosition().y >= stopChaseLimit)
	{
		setTarget(0);
		if (imTheOwner())decisionTimer = 0;
		return;
	}
	if (targetPosition != mChaseTarget->getPosition())setTarget(mChaseTarget->getPosition() + (antiGravity ? Vector3(0, 12, 0) : Vector3::ZERO), false);
	if (!hasAttack)return;
	const Real tAttackRange = (Real)(length <= 55.55 ? 2500 : (length*length*0.81));
	if (!isAttacking() && mObjectNode->getPosition().squaredDistance(mChaseTarget->getPosition())<tAttackRange)isReadyToAttack = true;
}
void MagixCritter::updateMovement(const FrameEvent &evt)
{
	const Vector3 tPosition = mObjectNode->getPosition();
	Real tDistance = tPosition.squaredDistance(targetPosition);
	if (tDistance>0)
	{
		//Lateral movement
		const Real tX = targetPosition.x - tPosition.x;
		const Real tZ = targetPosition.z - tPosition.z;

		tDistance = Vector2(tX, tZ).squaredLength();

		if (tDistance>(maxSpeed / 25))
		{
			Radian tYaw = Degree(0);
			if (tZ != 0)tYaw = Math::ATan(tX / tZ);
			else tYaw = Degree((Real)(tX >= 0 ? 90 : 270));
			if (tX<0)tYaw += Degree(360);
			if (tZ<0)tYaw += Degree(180);

			if (tYaw<Degree(0))tYaw += Degree(360);
			if (tYaw >= Degree(360))tYaw -= Degree(360);

			Vector2 tVect = Vector2(0, 0);
			if (tYaw == Degree(0) || tYaw == Degree(180))
			{
				tVect.y = (Real)(10 * (tYaw == Degree(180) ? -1 : 1));
			}
			else if (tYaw == Degree(90) || tYaw == Degree(270))
			{
				tVect.x = (Real)(10 * (tYaw == Degree(270) ? -1 : 1));
			}
			else
			{
				tVect.y = 10 * Math::Cos(tYaw);
				tVect.x = 10 * Math::Sin(tYaw);
			}
			addForce(Vector3(tVect.x*evt.timeSinceLastFrame, 0, tVect.y*evt.timeSinceLastFrame));
		}
		else if (tDistance>0)
		{
			lastPosition = tPosition;
			mObjectNode->setPosition(targetPosition.x, mObjectNode->getPosition().y, targetPosition.z);
			mForce.x = 0;
			mForce.z = 0;
		}

		//Vertical movement
		if (antiGravity)
		{
			const Real tYDistance = targetPosition.y - tPosition.y;
			if (Math::Abs(tYDistance)>1)
			{
				addForce(Vector3(0, 5 * (tYDistance>0 ? 1 : -1)*evt.timeSinceLastFrame, 0));
			}
			else if (Math::Abs(tYDistance)>0)
			{
				lastPosition = tPosition;
				mObjectNode->setPosition(mObjectNode->getPosition().x, targetPosition.y, mObjectNode->getPosition().z);
				mForce.y = 0;
			}
		}
	}
}
void MagixCritter::updateDeath(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag)
{
	if (!isDead)
	{
		if (imTheOwner() && !hasSentDeath)
		{
			hasSentDeath = true;
			killedFlag = true;
		}
		return;
	}
	if (decisionTimer>0)decisionTimer -= evt.timeSinceLastFrame;
	if (decisionTimer<0)decisionTimer = 0;
	if (decisionTimer == 0)
	{
		if (isDecomposing)decomposedFlag = true;
		else
		{
			isDecomposing = true;
			decisionTimer = 1;
		}
	}
	if (isDecomposing)setFullColour(ColourValue(1, 0.95, 0.75, (float)(decisionTimer*0.5)));
}
void MagixCritter::setFullColour(const ColourValue &fullColour)
{
	mEnt->setMaterialName("FullModColour");

	mEnt->getSubEntity(0)->setCustomParameter(1, Vector4(fullColour.r, fullColour.g, fullColour.b, fullColour.a));

#pragma warning(push)
#pragma warning(disable : 4482)
	//If fragment programs are not supported
	if (mEnt->getSubEntity(0)->getTechnique()->getName() == "2")
	{
		mEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setColourOperationEx(LayerBlendOperationEx::LBX_SOURCE1,
			LayerBlendSource::LBS_MANUAL,
			LayerBlendSource::LBS_CURRENT,
			fullColour);
		mEnt->getSubEntity(0)->getTechnique()->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LayerBlendOperationEx::LBX_SOURCE1,
			LayerBlendSource::LBS_MANUAL,
			LayerBlendSource::LBS_CURRENT,
			fullColour.a);
	}
#pragma warning(pop)
}
void MagixCritter::kill()
{
	hp = 0;
	isDead = true;
	decisionTimer = (Real)(isDrawPoint ? 1 : 10);
	if (antiGravity)antiGravity = false;
	if (hasAttack)attackHasEnded = true;
}
void MagixCritter::setTarget(const Vector3 &position, bool clearChaseTarget)
{
	if (clearChaseTarget)
	{
		mChaseTarget = 0;
		isReadyToAttack = false;
	}
	targetPosition = position;

	//Yaw to target
	if (targetPosition.x != mObjectNode->getPosition().x || targetPosition.z != mObjectNode->getPosition().z)
	{
		const Real tX = targetPosition.x - mObjectNode->getPosition().x;
		const Real tZ = targetPosition.z - mObjectNode->getPosition().z;
		Radian tYaw = Degree(0);
		if (tZ != 0)tYaw = Math::ATan(tX / tZ);
		else tYaw = Degree((Real)(tX >= 0 ? 90 : 270));
		if (tX<0)tYaw += Degree(360);
		if (tZ<0)tYaw += Degree(180);
		setYaw(Degree(tYaw));
	}
}
void MagixCritter::setTarget(MagixObject *target, const unsigned char &attID)
{
	mChaseTarget = target;
	isReadyToAttack = false;
	attackID = attID;
	updateChase();
}
const Vector3 MagixCritter::getTarget()
{
	return targetPosition;
}
void MagixCritter::setDecisionTimer(const Real &value)
{
	decisionTimer = value;
}
const Real MagixCritter::getDecisionTimer()
{
	return decisionTimer;
}
bool MagixCritter::getIsDead()
{
	return isDead;
}
void MagixCritter::setRoamArea(bool flag)
{
	hasRoamArea = flag;
}
bool MagixCritter::getRoamArea()
{
	return hasRoamArea;
}
void MagixCritter::setRoamAreaID(const unsigned char &iID)
{
	roamAreaID = iID;
}
const unsigned char MagixCritter::getRoamAreaID()
{
	return roamAreaID;
}
void MagixCritter::setWorldCritterID(const unsigned char &iID)
{
	worldCritterID = iID;
	isWorldCritter = true;
}
bool MagixCritter::getIsWorldCritter()
{
	return isWorldCritter;
}
const unsigned char MagixCritter::getWorldCritterID()
{
	return worldCritterID;
}
const Vector3 MagixCritter::getPosition(bool headPosition)
{
	if (!mObjectNode)return Vector3::ZERO;
	return (mObjectNode->getPosition() + (headPosition ? Vector3(0, (Real)(height*0.5), 0) : Vector3::ZERO));
}
const Real MagixCritter::getHeight()
{
	return height;
}
const Real MagixCritter::getLength()
{
	return length;
}
void MagixCritter::resetSize()
{
	height = 0;
	length = 0;
}
void MagixCritter::setIsPet(bool flag)
{
	isPet = flag;
	if (flag)alliance = ALLIANCE_FRIEND;
}
bool MagixCritter::getIsPet()
{
	return isPet;
}
void MagixCritter::setHasAttack(bool flag)
{
	hasAttack = flag;
}
bool MagixCritter::getHasAttack()
{
	return hasAttack;
}
bool MagixCritter::popAttackHasEnded()
{
	const bool tFlag = attackHasEnded;
	attackHasEnded = false;
	return tFlag;
}
bool MagixCritter::popIsReadyToAttack()
{
	const bool tFlag = isReadyToAttack;
	isReadyToAttack = false;
	return tFlag;
}
const unsigned char MagixCritter::getAttackID()
{
	return attackID;
}
void MagixCritter::setStopChaseLimit(const Real &y)
{
	stopChaseLimit = y;
}
MagixObject* MagixCritter::getChaseTarget()
{
	return mChaseTarget;
}
void MagixCritter::setSoundTimer(const Real &timer)
{
	soundTimer = timer;
}
const Real MagixCritter::getSoundTimer()
{
	return soundTimer;
}
void MagixCritter::setIsDrawPoint(bool flag)
{
	isDrawPoint = flag;
}
bool MagixCritter::getIsDrawPoint()
{
	return isDrawPoint;
}