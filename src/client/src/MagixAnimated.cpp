#include "MagixAnimated.h"
#include "MagixConst.h"
#include "DebugTrace.h"
#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
MagixAnimated::MagixAnimated()
{
	mObjectNode = 0;
	objectID = 0;
	mForce = Vector3::ZERO;
	groundHeight = 0;
	maxSpeed = 100;
	speedMultiplier = 1;
	lastPosition = Vector3::ZERO;
	updateTime = 0;
	faceDirectionYaw = Degree(0);
	antiGravity = false;
	penaltyVelocity = Vector3::ZERO;
	isHidden = false;
	record = 0;
}
MagixAnimated::~MagixAnimated()
{
}
void MagixAnimated::createObject(const unsigned short &iID, SceneManager *sceneMgr, const String &objectMesh, const String &matName)
{
	if (objectID)destroyObject(sceneMgr);
	objectID = iID;

	Entity *objectEnt = sceneMgr->createEntity("Object" + StringConverter::toString(iID), objectMesh + ".mesh");
	objectEnt->setQueryFlags(OBJECT_MASK);

	objectEnt->setMaterialName(matName);

	if (!mObjectNode)mObjectNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mObjectNode->attachObject(objectEnt);
	mObjectNode->setScale(5, 5, 5);
}
void MagixAnimated::destroyObject(SceneManager *sceneMgr)
{
	if (!objectID)return;

	if (mObjectNode)
	{
		mObjectNode->detachAllObjects();
		sceneMgr->destroySceneNode(mObjectNode->getName());
	}
	if (sceneMgr->hasEntity("Object" + StringConverter::toString(objectID)))sceneMgr->destroyEntity("Object" + StringConverter::toString(objectID));

	mObjectNode = 0;
	objectID = 0;
	isHidden = false;
}
void MagixAnimated::updateAnimation(const FrameEvent &evt)
{
	if (!objectID)return;
	//object animation
}
void MagixAnimated::update(const FrameEvent &evt)
{
	if (!objectID)return;

	updatePhysics(evt);
	updateAnimation(evt);
	updateFaceDirection(evt);
}
void MagixAnimated::updateFaceDirection(const FrameEvent &evt)
{
	Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
	if (tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
	if (tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
	Degree tYaw = faceDirectionYaw - tCurrentYaw;
	if (Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (Real)(tYaw<Degree(0) ? -1 : 1);
	if (Math::Abs(tYaw)>Degree(0.1))tYaw *= (5 * evt.timeSinceLastFrame<1 ? 5 * evt.timeSinceLastFrame : 1);
	if (tYaw != Degree(0))mObjectNode->yaw(tYaw);
}
void MagixAnimated::updatePhysics(const FrameEvent &evt)
{
	if (distanceFromGround() != 0/*record != 0*/) {
//		DBOUT(record << " -- 1 mForce.y: " << mForce.y << std::endl);
	}
	if (distanceFromGround()!=0/*record != 0*/) {
//		DBOUT(record << " -- 2 mForce.y: " << mForce.y << std::endl);
	}
	if (distanceFromGround()!=0/*record != 0*/) {
//		DBOUT(record << " -- 3 mForce.y: " << mForce.y << std::endl);
	}
	if (distanceFromGround() != 0/*record != 0*/) {
//		DBOUT(record << " -- 4 mForce.y: " << mForce.y << std::endl);
	}
	if (distanceFromGround() != 0/*record != 0*/) {
//		DBOUT(record << " -- 5 mForce.y: " << mForce.y << std::endl);
	}
	//apply forces to node
	lastPosition = mObjectNode->getPosition();
	mObjectNode->translate(mForce);
	//Friction
	if (mForce.x != 0 || mForce.z != 0)
	{
		Vector2 tVect = Vector2(0, 0);
		if (mForce.x == 0)
		{
			tVect.y = FRICTION;
		}
		else if (mForce.z == 0)
		{
			tVect.x = FRICTION;
		}
		else
		{
			Radian tAngle = Math::ATan(Math::Abs(mForce.z) / Math::Abs(mForce.x));
			tVect.x = FRICTION * Math::Cos(tAngle);
			tVect.y = FRICTION * Math::Sin(tAngle);
		}
		if (Math::Abs(mForce.x)<tVect.x*evt.timeSinceLastFrame)mForce.x = 0;
		else mForce.x -= tVect.x * (mForce.x<0 ? -1 : 1) *evt.timeSinceLastFrame;
		if (Math::Abs(mForce.z)<tVect.y*evt.timeSinceLastFrame)mForce.z = 0;
		else mForce.z -= tVect.y * (mForce.z<0 ? -1 : 1) *evt.timeSinceLastFrame;
	}
	//Speed limit
	if (Vector2(mForce.x, mForce.z).squaredLength()>Math::Sqr(maxSpeed*speedMultiplier*evt.timeSinceLastFrame))
	{
		Radian tAngle = Math::ATan(Math::Abs(mForce.z) / Math::Abs(mForce.x));
		mForce.x = maxSpeed * speedMultiplier * (mForce.x<0 ? -1 : 1) * Math::Cos(tAngle) *evt.timeSinceLastFrame;
		mForce.z = maxSpeed * speedMultiplier * (mForce.z<0 ? -1 : 1) * Math::Sin(tAngle) *evt.timeSinceLastFrame;
	}
	//Gravity
	if (!antiGravity && distanceFromGround()>0)
	{
		mForce.y -= GRAVITY *evt.timeSinceLastFrame;
	}
	//Air resistance
	else if (antiGravity)
	{
		if (mForce.y > 0)
		{
			mForce.y -= FRICTION *evt.timeSinceLastFrame;
			if (mForce.y<0)mForce.y = 0;
		}
		else if (mForce.y < 0)
		{
			mForce.y += FRICTION *evt.timeSinceLastFrame;
			if (mForce.y>0)mForce.y = 0;
		}
	}
	if (penaltyVelocity != Vector3::ZERO)
	{
		mObjectNode->translate(penaltyVelocity);
		penaltyVelocity = Vector3::ZERO;
	}
	//Terminal Velocity
	/*if (Math::Abs(mForce.y)>TERMINAL_VEL*evt.timeSinceLastFrame*(antiGravity ? 0.5 : 1))
	{
	mForce.y = (Real)(TERMINAL_VEL * (mForce.y < 0 ? -1 : 1)*evt.timeSinceLastFrame*(antiGravity ? 0.5 : 1));
	}*/
	if (distanceFromGround() != 0/*record != 0*/) {
//		DBOUT(record << " -- 6 mForce.y: " << mForce.y << std::endl);
	}
	//clip to ground level if below ground or within ground threshold
	if (!antiGravity && (distanceFromGround()<0 || (distanceFromGround() <= GROUND_THRESHOLD && mForce.y <= 0)))
	{
		mForce.y = 0;
		mObjectNode->setPosition(mObjectNode->getPosition().x, groundHeight, mObjectNode->getPosition().z);
	}
	else if (distanceFromGround()<0)
	{
		mObjectNode->setPosition(mObjectNode->getPosition().x, groundHeight, mObjectNode->getPosition().z);
	}
	if (distanceFromGround() != 0/*record != 0*/) {
//		DBOUT(record << " -- 7 mForce.y: " << mForce.y << std::endl);
		record++;
	}
}
/*typedef struct {
        PVOID trc[10];
        ULONG len;
        ULONG cnt;
} stack_t;*/
void MagixAnimated::addForce(const Vector3 &value)
{
	mForce += value;
	//printStack();
	DBOUT("++++addForce: mForce.y: " << mForce.y << std::endl);
	record = 0;
}
void MagixAnimated::setYForce(const Real &value)
{
	mForce.y = value;
}
void MagixAnimated::setForce(const Vector3 &value)
{
	mForce = value;
}
const Vector3 MagixAnimated::getForce()
{
	return mForce;
}
void MagixAnimated::setPosition(const Real &x, const Real &y, const Real &z)
{
	if (mObjectNode)mObjectNode->setPosition(x, y, z);
}
void MagixAnimated::setPosition(const Vector3 &position)
{
	if (mObjectNode)mObjectNode->setPosition(position);
}
const Vector3 MagixAnimated::getPosition()
{
	if (mObjectNode)return mObjectNode->getPosition();
	return Vector3::ZERO;
}
SceneNode* MagixAnimated::getObjectNode()
{
	return mObjectNode;
}
void MagixAnimated::setGroundHeight(const Real &value)
{
	groundHeight = value;
}
const Real MagixAnimated::getGroundHeight()
{
	return groundHeight;
}
const Real MagixAnimated::distanceFromGround()
{
	if (mObjectNode)return (mObjectNode->getPosition().y - groundHeight);
	return 0;
}
const OBJECT_TYPE MagixAnimated::getType()
{
	return OBJECT_ANIMATED;
}
bool MagixAnimated::isPositionChanged(bool excludeYAxis)
{
	return (excludeYAxis ? (lastPosition.x != mObjectNode->getPosition().x || lastPosition.z != mObjectNode->getPosition().z)
		: lastPosition != mObjectNode->getPosition());
}
const Vector3 MagixAnimated::getLastPosition()
{
	return lastPosition;
}
bool MagixAnimated::isMovingReverse()
{
	return (Degree(Math::ACos(Vector2(mObjectNode->getOrientation().zAxis().x, mObjectNode->getOrientation().zAxis().z).dotProduct(Vector2(mForce.x, mForce.z) * 100))) >= Degree(180));
}
void MagixAnimated::yaw(const Degree &angle, bool instant)
{
	faceDirectionYaw += angle;
	if (faceDirectionYaw<Degree(0))faceDirectionYaw += Degree(360);
	if (faceDirectionYaw >= Degree(360))faceDirectionYaw -= Degree(360);
	if (instant)
	{
		Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
		if (tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
		if (tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
		Degree tYaw = faceDirectionYaw - tCurrentYaw;
		if (Math::Abs(tYaw)>Degree(180))
			tYaw -= Degree(360) * (Real)(tYaw<Degree(0) ? -1 : 1);
		mObjectNode->yaw(tYaw);
	}
}
void MagixAnimated::setYaw(const Degree &angle, bool instant)
{
	faceDirectionYaw = angle;
	if (faceDirectionYaw<Degree(0))faceDirectionYaw += Degree(360);
	if (faceDirectionYaw >= Degree(360))faceDirectionYaw -= Degree(360);
	if (instant)
	{
		Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
		if (tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
		if (tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
		Degree tYaw = faceDirectionYaw - tCurrentYaw;
		if (Math::Abs(tYaw)>Degree(180))
			tYaw -= Degree(360) * (Real)(tYaw<Degree(0) ? -1 : 1);
		mObjectNode->yaw(tYaw);
	}
}
const Degree MagixAnimated::getYaw()
{
	return faceDirectionYaw;
}
const Quaternion MagixAnimated::getFaceDirection()
{
	Quaternion tOrig = mObjectNode->getOrientation();
	Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
	if (tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
	if (tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
	Degree tYaw = faceDirectionYaw - tCurrentYaw;
	if (Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (Real)(tYaw<Degree(0) ? -1 : 1);
	mObjectNode->yaw(tYaw);
	Quaternion tEnd = mObjectNode->getOrientation();
	mObjectNode->setOrientation(tOrig);
	return tEnd;
}
void MagixAnimated::setAntiGravity(bool flag)
{
	antiGravity = flag;
}
bool MagixAnimated::isAntiGravity()
{
	return antiGravity;
}
void MagixAnimated::toggleAntiGravity()
{
	antiGravity = !antiGravity;
}
void MagixAnimated::addPenaltyVelocity(const Vector3 &value)
{
	penaltyVelocity += value;
}
void MagixAnimated::setSpeedMultiplier(const Real &value)
{
	speedMultiplier = value;
}
const Real MagixAnimated::getSpeedMultiplier()
{
	return speedMultiplier;
}
void MagixAnimated::setMaxSpeed(const Real &value)
{
	maxSpeed = value;
}