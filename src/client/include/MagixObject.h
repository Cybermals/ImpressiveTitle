#ifndef __MagixObject_h_
#define __MagixObject_h_

#define FRICTION 3
#define GRAVITY 4
#define TERMINAL_VEL 200
#define GROUND_THRESHOLD 2

#define ALLIANCE_PLAYER 1
#define ALLIANCE_FRIEND 2
#define ALLIANCE_ENEMY 3

enum OBJECT_TYPE
{
	OBJECT_BASIC,
	OBJECT_ANIMATED,
	OBJECT_LIVING,
	OBJECT_UNIT,
	OBJECT_PLAYER,
	OBJECT_ITEM,
	OBJECT_CRITTER
};
struct HitInfo
{
	unsigned short ID;
	Real hp;
	Vector3 force;
	bool isMine;
	HitInfo(const unsigned short &i, const Real &h, const Vector3 &f, bool iM=false)
	{
		ID = i;
		hp = h;
		force = f;
		isMine = iM;
	}
};

using namespace Ogre;

class MagixObject
{
protected:
	SceneNode *mObjectNode;
public:
	MagixObject()
	{
		mObjectNode = 0;
	}
	~MagixObject()
	{
	}
	virtual const OBJECT_TYPE getType()
	{
		return OBJECT_BASIC;
	}
	virtual const Vector3 getPosition()
	{
		if(mObjectNode)return mObjectNode->getPosition();
		return Vector3::ZERO;
	}
	virtual SceneNode* getObjectNode()
	{
		return mObjectNode;
	}
};

class MagixAnimated : public MagixObject
{
protected:
	unsigned short objectID;
	Vector3 mForce;
	Real groundHeight;
	Real maxSpeed;
	Real speedMultiplier;
	Vector3 lastPosition;
	Real updateTime;
	Degree faceDirectionYaw;
	bool antiGravity;
	Vector3 penaltyVelocity;
public:
	bool isHidden;
	MagixAnimated()
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
	}
	~MagixAnimated()
	{
	}
	void createObject(const unsigned short &iID, SceneManager *sceneMgr, const String &objectMesh, const String &matName)
	{
		if(objectID)destroyObject(sceneMgr);
		objectID = iID;

		Entity *objectEnt = sceneMgr->createEntity("Object"+StringConverter::toString(iID), objectMesh+".mesh");
		objectEnt->setQueryFlags(OBJECT_MASK);

		objectEnt->setMaterialName(matName);

		if(!mObjectNode)mObjectNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mObjectNode->attachObject(objectEnt);
		mObjectNode->setScale(5,5,5);
	}
	void destroyObject(SceneManager *sceneMgr)
	{
		if(!objectID)return;

		if(mObjectNode)
		{
			mObjectNode->detachAllObjects();
			sceneMgr->destroySceneNode(mObjectNode->getName());
		}
		if(sceneMgr->hasEntity("Object"+StringConverter::toString(objectID)))sceneMgr->destroyEntity("Object"+StringConverter::toString(objectID));

		mObjectNode = 0;
		objectID = 0;
		isHidden = false;
	}
	virtual void updateAnimation(const FrameEvent &evt)
	{
		if(!objectID)return;
		//object animation
	}
	virtual void update(const FrameEvent &evt)
	{
		if(!objectID)return;

		updatePhysics(evt);
		updateAnimation(evt);
		updateFaceDirection(evt);
	}
	void updateFaceDirection(const FrameEvent &evt)
	{
		Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
		if(tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
		if(tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
		Degree tYaw = faceDirectionYaw - tCurrentYaw;
		if(Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (tYaw<Degree(0)?-1:1);
		if(Math::Abs(tYaw)>Degree(0.1))tYaw *= (5*evt.timeSinceLastFrame<1?5*evt.timeSinceLastFrame:1);
		if(tYaw!=Degree(0))mObjectNode->yaw(tYaw);
	}
	void updatePhysics(const FrameEvent &evt)
	{
		//Friction
		if(mForce.x!=0 || mForce.z!=0)
		{
			Vector2 tVect = Vector2(0,0);
			if(mForce.x==0)
			{
				tVect.y = FRICTION;
			}
			else if(mForce.z==0)
			{
				tVect.x = FRICTION;
			}
			else
			{
				Radian tAngle = Math::ATan(Math::Abs(mForce.z)/Math::Abs(mForce.x));
				tVect.x = FRICTION * Math::Cos(tAngle);
				tVect.y = FRICTION * Math::Sin(tAngle);
			}
			if(Math::Abs(mForce.x)<tVect.x*evt.timeSinceLastFrame)mForce.x = 0;
			else mForce.x -= tVect.x * (mForce.x<0?-1:1) *evt.timeSinceLastFrame;
			if(Math::Abs(mForce.z)<tVect.y*evt.timeSinceLastFrame)mForce.z = 0;
			else mForce.z -= tVect.y * (mForce.z<0?-1:1) *evt.timeSinceLastFrame;
		}

		//Gravity
		if(!antiGravity && distanceFromGround()>0)
		{
			mForce.y -= GRAVITY *evt.timeSinceLastFrame;
		}
		//Air resistance
		else if(antiGravity)
		{
			if(mForce.y > 0)
			{
				mForce.y -= FRICTION *evt.timeSinceLastFrame;
				if(mForce.y<0)mForce.y = 0;
			}
			else if(mForce.y < 0)
			{
				mForce.y += FRICTION *evt.timeSinceLastFrame;
				if(mForce.y>0)mForce.y = 0;
			}
		}

		//Speed limit
		if(Vector2(mForce.x,mForce.z).squaredLength()>Math::Sqr(maxSpeed*speedMultiplier*evt.timeSinceLastFrame))
		{
			Radian tAngle = Math::ATan(Math::Abs(mForce.z)/Math::Abs(mForce.x));
			mForce.x = maxSpeed * speedMultiplier * (mForce.x<0?-1:1) * Math::Cos(tAngle) *evt.timeSinceLastFrame;
			mForce.z = maxSpeed * speedMultiplier * (mForce.z<0?-1:1) * Math::Sin(tAngle) *evt.timeSinceLastFrame;
		}
		//Terminal Velocity
		if(Math::Abs(mForce.y)>TERMINAL_VEL*evt.timeSinceLastFrame*(antiGravity?0.5:1))mForce.y = TERMINAL_VEL * (mForce.y<0?-1:1)*evt.timeSinceLastFrame*(antiGravity?0.5:1);
		
		//apply forces to node
		lastPosition = mObjectNode->getPosition();
		mObjectNode->translate(mForce);
		if(penaltyVelocity!=Vector3::ZERO)
		{
			mObjectNode->translate(penaltyVelocity);
			penaltyVelocity = Vector3::ZERO;
		}

		//clip to ground level if below ground or within ground threshold
		if(!antiGravity && (distanceFromGround()<0 || (distanceFromGround()<=GROUND_THRESHOLD && mForce.y<=0)))
		{
			mForce.y = 0;
			mObjectNode->setPosition(mObjectNode->getPosition().x, groundHeight, mObjectNode->getPosition().z);
		}
		else if(distanceFromGround()<0)
		{
			mObjectNode->setPosition(mObjectNode->getPosition().x, groundHeight, mObjectNode->getPosition().z);
		}
	}
	void addForce(const Vector3 &value)
	{
		mForce += value;
	}
	void setForce(const Vector3 &value)
	{
		mForce = value;
	}
	const Vector3 getForce()
	{
		return mForce;
	}
	void setPosition(const Real &x, const Real &y, const Real &z)
	{
		if(mObjectNode)mObjectNode->setPosition(x,y,z);
	}
	void setPosition(const Vector3 &position)
	{
		if(mObjectNode)mObjectNode->setPosition(position);
	}
	virtual const Vector3 getPosition()
	{
		if(mObjectNode)return mObjectNode->getPosition();
		return Vector3::ZERO;
	}
	SceneNode* getObjectNode()
	{
		return mObjectNode;
	}
	void setGroundHeight(const Real &value)
	{
		groundHeight = value;
	}
	const Real getGroundHeight()
	{
		return groundHeight;
	}
	const Real distanceFromGround()
	{
		if(mObjectNode)return (mObjectNode->getPosition().y - groundHeight);
		return 0;
	}
	virtual const OBJECT_TYPE getType()
	{
		return OBJECT_ANIMATED;
	}
	bool isPositionChanged(bool excludeYAxis=false)
	{
		return (excludeYAxis? (lastPosition.x!=mObjectNode->getPosition().x || lastPosition.z!=mObjectNode->getPosition().z)
			:lastPosition!=mObjectNode->getPosition() );
	}
	const Vector3 getLastPosition()
	{
		return lastPosition;
	}
	bool isMovingReverse()
	{
		return (Degree(Math::ACos(Vector2(mObjectNode->getOrientation().zAxis().x,mObjectNode->getOrientation().zAxis().z).dotProduct(Vector2(mForce.x,mForce.z)*100))) >= Degree(180));
	}
	void yaw(const Degree &angle, bool instant=false)
	{
		faceDirectionYaw += angle;
		if(faceDirectionYaw<Degree(0))faceDirectionYaw += Degree(360);
		if(faceDirectionYaw>=Degree(360))faceDirectionYaw -= Degree(360);
		if(instant)
		{
			Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
			if(tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
			if(tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
			Degree tYaw = faceDirectionYaw - tCurrentYaw;
			if(Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (tYaw<Degree(0)?-1:1);
			mObjectNode->yaw(tYaw);
		}
	}
	void setYaw(const Degree &angle, bool instant=false)
	{
		faceDirectionYaw = angle;
		if(faceDirectionYaw<Degree(0))faceDirectionYaw += Degree(360);
		if(faceDirectionYaw>=Degree(360))faceDirectionYaw -= Degree(360);
		if(instant)
		{
			Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
			if(tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
			if(tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
			Degree tYaw = faceDirectionYaw - tCurrentYaw;
			if(Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (tYaw<Degree(0)?-1:1);
			mObjectNode->yaw(tYaw);
		}
	}
	const Degree getYaw()
	{
		return faceDirectionYaw;
	}
	const Quaternion getFaceDirection()
	{
		Quaternion tOrig = mObjectNode->getOrientation();
		Degree tCurrentYaw = Degree(mObjectNode->getOrientation().getYaw());
		if(tCurrentYaw < Degree(0))tCurrentYaw += Degree(360);
		if(tCurrentYaw > Degree(360))tCurrentYaw -= Degree(360);
		Degree tYaw = faceDirectionYaw - tCurrentYaw;
		if(Math::Abs(tYaw)>Degree(180))tYaw -= Degree(360) * (tYaw<Degree(0)?-1:1);
		mObjectNode->yaw(tYaw);
		Quaternion tEnd = mObjectNode->getOrientation();
		mObjectNode->setOrientation(tOrig);
		return tEnd;
	}
	void setAntiGravity(bool flag)
	{
		antiGravity = flag;
	}
	bool isAntiGravity()
	{
		return antiGravity;
	}
	void toggleAntiGravity()
	{
		antiGravity = !antiGravity;
	}
	void addPenaltyVelocity(const Vector3 &value)
	{
		penaltyVelocity += value;
	}
	void setSpeedMultiplier(const Real &value)
	{
		speedMultiplier = value;
	}
	const Real getSpeedMultiplier()
	{
		return speedMultiplier;
	}
	void setMaxSpeed(const Real &value)
	{
		maxSpeed = value;
	}
};

class MagixLiving : public MagixAnimated
{
protected:
	short maxHP;
	short hp;
	unsigned char alliance;
	bool invulnerable;
public:
	bool isHidden;
	MagixLiving()
	{
		maxHP = 0;
		hp = 0;
		alliance = 0;
		invulnerable = false;
		isHidden = false;
	}
	~MagixLiving()
	{
	}
	virtual void setHP(const short &iMaxHP, const Real &ratio=1)
	{
		maxHP = iMaxHP;
		hp = ratio*maxHP;
	}
	virtual void setHPRatio(const Real &ratio)
	{
		hp = ratio*maxHP;
	}
	virtual bool addHP(const short &value)
	{
		if(invulnerable && value<0)return false;
		hp += value;
		if(hp>maxHP)hp = maxHP;
		if(hp<0)hp = 0;
		return (hp==0);
	}
	const Real getHPRatio()
	{
		if(maxHP<=0)return 0;
		return Real(hp)/Real(maxHP);
	}
	const short getHP()
	{
		return hp;
	}
	const short getMaxHP()
	{
		return maxHP;
	}
	void setAlliance(const unsigned char &ally)
	{
		alliance = ally;
	}
	const unsigned char getAlliance()
	{
		return alliance;
	}
	bool matchAlliance(const unsigned char &ally)
	{
		if(alliance==ally)return true;
		if(alliance==ALLIANCE_PLAYER)
		{
			if(ally==ALLIANCE_FRIEND)return true;
		}
		if(alliance==ALLIANCE_FRIEND)
		{
			if(ally==ALLIANCE_PLAYER)return true;
		}
		return false;
	}
	virtual const OBJECT_TYPE getType()
	{
		return OBJECT_CRITTER;
	}
	bool isInvulnerable()
	{
		return invulnerable;
	}
	void setInvulnerable(bool flag)
	{
		invulnerable = flag;
	}
	virtual const Real getHeight()
	{
		return 0;
	}
};


#endif