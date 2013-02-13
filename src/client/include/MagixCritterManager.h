#ifndef __MagixCritterManager_h_
#define __MagixCritterManager_h_

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

		setAnimation("Idle",0.2,true);
	}
	~MagixCritter()
	{
	}
	const OBJECT_TYPE getType()
	{
		return OBJECT_CRITTER;
	}
	const unsigned short getID()
	{
		return ID;
	}
	const String getCritterType()
	{
		return type;
	}
	void setOwnerIndex(const short &index)
	{
		ownerIndex = index;
	}
	const short getOwnerIndex()
	{
		return ownerIndex;
	}
	void setImTheOwner()
	{
		ownerIndex = 0;
	}
	bool imTheOwner()
	{
		return (ownerIndex==0);
	}
	void setLastHitIndex(const short &index)
	{
		lastHitIndex = index;
	}
	const short getLastHitIndex()
	{
		return lastHitIndex;
	}
	Entity* getEnt()
	{
		return mEnt;
	}
	void update(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag)
	{
		if(!isDead && (hp>0||isPet))
		{
			if(imTheOwner())updateDecision(evt);
			updateChase();
			updateMovement(evt);
			if(isPet)
			{
				if(hp<=0)updateDeath(evt,killedFlag,decomposedFlag);
				else if(hasSentDeath)hasSentDeath = false;
			}
		}
		else updateDeath(evt,killedFlag,decomposedFlag);
		updatePhysics(evt);
		updateAction();
		updateAnimation(evt);
		updateFaceDirection(evt);
		updateSound(evt);
		if(height==0)height = mEnt->getWorldBoundingBox().getMaximum().y - mEnt->getWorldBoundingBox().getMinimum().y;
		if(length==0)length = mEnt->getWorldBoundingBox().getMaximum().z - mEnt->getWorldBoundingBox().getMinimum().z;
	}
	void updateSound(const FrameEvent &evt)
	{
		if(soundTimer>0)soundTimer -= evt.timeSinceLastFrame;
		if(soundTimer<0)soundTimer = 0;
	}
	void updateAnimation(const FrameEvent &evt)
	{
		if(!mEnt->isVisible())return;
		if(anim!="")mEnt->getAnimationState(anim)->addTime(evt.timeSinceLastFrame*animSpeed);
	}
	void updateAction()
	{
		if(isDead){setAnimation("Die",1,false); return;}
		if(hasAttack && isAttacking())
		{
			if(mEnt->getAnimationState(anim)->hasEnded())attackHasEnded = true;
			else return;
		}
		if(mForce.x!=0||mForce.z!=0){setAnimation("Run",1,true); return;}
		if(anim==""||anim=="Run"||mEnt->getAnimationState(anim)->hasEnded())
		setAnimation("Idle",0.2,true);
	}
	bool isAttacking()
	{
		return (StringUtil::startsWith(anim,"Attack",false));
	}
	const String getAnim()
	{
		return anim;
	}
	bool setAnimation(const String &type, const Real &speed=1, bool loop=false)
	{
		if(anim==type)return false;
		if(!mEnt->getSkeleton()->hasAnimation(type))return false;
		if(anim!="")mEnt->getAnimationState(anim)->setEnabled(false);
		anim = type;
		animSpeed = speed;
		mEnt->getAnimationState(anim)->setEnabled(true);
		mEnt->getAnimationState(anim)->setLoop(loop);
		mEnt->getAnimationState(anim)->setTimePosition(0);
		return true;
	}
	void updateDecision(const FrameEvent &evt)
	{
		if(decisionTimer>0)decisionTimer -= evt.timeSinceLastFrame;
		if(decisionTimer<0)decisionTimer = 0;
		if(decisionTimer>0)return;
	}
	void updateChase()
	{
		if(!mChaseTarget)return;
		if(stopChaseLimit>0 && mChaseTarget->getPosition().y >= stopChaseLimit)
		{
			setTarget(0);
			if(imTheOwner())decisionTimer = 0;
			return;
		}
		if(targetPosition != mChaseTarget->getPosition())setTarget(mChaseTarget->getPosition()+(antiGravity?Vector3(0,12,0):Vector3::ZERO),false);
		if(!hasAttack)return;
		const Real tAttackRange = length<=55.55 ? 2500 : (length*length*0.81);
		if(!isAttacking() && mObjectNode->getPosition().squaredDistance(mChaseTarget->getPosition())<tAttackRange)isReadyToAttack = true;
	}
	void updateMovement(const FrameEvent &evt)
	{
		const Vector3 tPosition = mObjectNode->getPosition();
		Real tDistance = tPosition.squaredDistance(targetPosition);
		if(tDistance>0)
		{
			//Lateral movement
			const Real tX = targetPosition.x - tPosition.x;
			const Real tZ = targetPosition.z - tPosition.z;

			tDistance = Vector2(tX,tZ).squaredLength();
		
			if(tDistance>(maxSpeed/25))
			{
				Radian tYaw = Degree(0);
				if(tZ!=0)tYaw = Math::ATan(tX/tZ);
				else tYaw = Degree(tX>=0?90:270);
				if(tX<0)tYaw += Degree(360);
				if(tZ<0)tYaw += Degree(180);

				if(tYaw<Degree(0))tYaw += Degree(360);
				if(tYaw>=Degree(360))tYaw -= Degree(360);

				Vector2 tVect = Vector2(0,0);
				if(tYaw==Degree(0)||tYaw==Degree(180))
				{
					tVect.y = 10 * (tYaw==Degree(180)?-1:1);
				}
				else if(tYaw==Degree(90)||tYaw==Degree(270))
				{
					tVect.x = 10 * (tYaw==Degree(270)?-1:1);
				}
				else
				{
					tVect.y = 10 * Math::Cos(tYaw);
					tVect.x = 10 * Math::Sin(tYaw);
				}
				addForce(Vector3(tVect.x*evt.timeSinceLastFrame,0,tVect.y*evt.timeSinceLastFrame));
			}
			else if(tDistance>0)
			{
				lastPosition = tPosition;
				mObjectNode->setPosition(targetPosition.x,mObjectNode->getPosition().y,targetPosition.z);
				mForce.x = 0;
				mForce.z = 0;
			}

			//Vertical movement
			if(antiGravity)
			{
				const Real tYDistance = targetPosition.y - tPosition.y;
				if(Math::Abs(tYDistance)>1)
				{
					addForce(Vector3(0,5*(tYDistance>0?1:-1)*evt.timeSinceLastFrame,0));
				}
				else if(Math::Abs(tYDistance)>0)
				{
					lastPosition = tPosition;
					mObjectNode->setPosition(mObjectNode->getPosition().x,targetPosition.y,mObjectNode->getPosition().z);
					mForce.y = 0;
				}
			}
		}
	}
	void updateDeath(const FrameEvent &evt, bool &killedFlag, bool &decomposedFlag)
	{
		if(!isDead)
		{
			if(imTheOwner() && !hasSentDeath)
			{
				hasSentDeath = true;
				killedFlag = true;
			}
			return;
		}
		if(decisionTimer>0)decisionTimer -= evt.timeSinceLastFrame;
		if(decisionTimer<0)decisionTimer = 0;
		if(decisionTimer==0)
		{
			if(isDecomposing)decomposedFlag = true;
			else
			{
				isDecomposing = true;
				decisionTimer = 1;
			}
		}
		if(isDecomposing)setFullColour(ColourValue(1,0.95,0.75,decisionTimer*0.5));
	}
	void setFullColour(const ColourValue &fullColour)
	{
		mEnt->setMaterialName("FullModColour");

		mEnt->getSubEntity(0)->setCustomParameter(1,Vector4(fullColour.r,fullColour.g,fullColour.b,fullColour.a));

		#pragma warning(push)
		#pragma warning(disable : 4482)
		//If fragment programs are not supported
		if(mEnt->getSubEntity(0)->getTechnique()->getName()=="2")
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
	void kill()
	{
		hp = 0;
		isDead = true;
		decisionTimer = isDrawPoint?1:10;
		if(antiGravity)antiGravity = false;
		if(hasAttack)attackHasEnded = true;
	}
	void setTarget(const Vector3 &position, bool clearChaseTarget=true)
	{
		if(clearChaseTarget)
		{
			mChaseTarget = 0;
			isReadyToAttack = false;
		}
		targetPosition = position;
		
		//Yaw to target
		if(targetPosition.x!=mObjectNode->getPosition().x || targetPosition.z!=mObjectNode->getPosition().z)
		{
			const Real tX = targetPosition.x - mObjectNode->getPosition().x;
			const Real tZ = targetPosition.z - mObjectNode->getPosition().z;
			Radian tYaw = Degree(0);
			if(tZ!=0)tYaw = Math::ATan(tX/tZ);
			else tYaw = Degree(tX>=0?90:270);
			if(tX<0)tYaw += Degree(360);
			if(tZ<0)tYaw += Degree(180);
			setYaw(Degree(tYaw));
		}
	}
	void setTarget(MagixObject *target, const unsigned char &attID=0)
	{
		mChaseTarget = target;
		isReadyToAttack = false;
		attackID = attID;
		updateChase();
	}
	const Vector3 getTarget()
	{
		return targetPosition;
	}
	void setDecisionTimer(const Real &value)
	{
		decisionTimer = value;
	}
	const Real getDecisionTimer()
	{
		return decisionTimer;
	}
	bool getIsDead()
	{
		return isDead;
	}
	void setRoamArea(bool flag)
	{
		hasRoamArea = flag;
	}
	bool getRoamArea()
	{
		return hasRoamArea;
	}
	void setRoamAreaID(const unsigned char &iID)
	{
		roamAreaID = iID;
	}
	const unsigned char getRoamAreaID()
	{
		return roamAreaID;
	}
	void setWorldCritterID(const unsigned char &iID)
	{
		worldCritterID = iID;
		isWorldCritter = true;
	}
	bool getIsWorldCritter()
	{
		return isWorldCritter;
	}
	const unsigned char getWorldCritterID()
	{
		return worldCritterID;
	}
	const Vector3 getPosition(bool headPosition=false)
	{
		if(!mObjectNode)return Vector3::ZERO;
		return (mObjectNode->getPosition() + (headPosition?Vector3(0,height*0.5,0):Vector3::ZERO));
	}
	const Real getHeight()
	{
		return height;
	}
	const Real getLength()
	{
		return length;
	}
	void resetSize()
	{
		height = 0;
		length = 0;
	}
	void setIsPet(bool flag)
	{
		isPet = flag;
		if(flag)alliance = ALLIANCE_FRIEND;
	}
	bool getIsPet()
	{
		return isPet;
	}
	void setHasAttack(bool flag)
	{
		hasAttack = flag;
	}
	bool getHasAttack()
	{
		return hasAttack;
	}
	bool popAttackHasEnded()
	{
		const bool tFlag = attackHasEnded;
		attackHasEnded = false;
		return tFlag;
	}
	bool popIsReadyToAttack()
	{
		const bool tFlag = isReadyToAttack;
		isReadyToAttack = false;
		return tFlag;
	}
	const unsigned char getAttackID()
	{
		return attackID;
	}
	void setStopChaseLimit(const Real &y)
	{
		stopChaseLimit = y;
	}
	MagixObject* getChaseTarget()
	{
		return mChaseTarget;
	}
	void setSoundTimer(const Real &timer)
	{
		soundTimer = timer;
	}
	const Real getSoundTimer()
	{
		return soundTimer;
	}
	void setIsDrawPoint(bool flag)
	{
		isDrawPoint = flag;
	}
	bool getIsDrawPoint()
	{
		return isDrawPoint;
	}
};

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
	void initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixEffectsManager *effectsMgr, MagixCollisionManager *collMgr, MagixSoundManager *soundMgr, MagixWorld *world, MagixCamera *camera)
	{
		mSceneMgr = sceneMgr;
		mDef = def;
		mGameStateManager = gameStateMgr;
		mEffectsManager = effectsMgr;
		mCollisionManager = collMgr;
		mSoundManager = soundMgr;
		mWorld = world;
		mCamera = camera;
		mRayQuery = sceneMgr->createRayQuery( Ray() );
	}
	void reset()
	{
		sentID.clear();
		stationaryQueue.clear();
		decisionQueue.clear();
		killedQueue.clear();
		deadQueue.clear();
		hitQueue.clear();
		myCritters.clear();
		deleteAllCritters();
		critterCount = 0;
		weatherEffectRequest = "";
	}
	void update(const FrameEvent &evt, SceneNode *playerNode=0)
	{
		for(vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
		{
			MagixCritter *tCritter = *it;
			//Range check
			bool tIsInRange = isInRange(tCritter);
			bool visibilityFlipped = false;
			if(tIsInRange && !tCritter->getEnt()->isVisible())
			{
				visibilityFlipped = true;
				tCritter->getObjectNode()->setVisible(true);
				if(tCritter->getIsDrawPoint())mEffectsManager->createParticle(tCritter->getObjectNode(),"DrawPoint",-1);
			}
			else if(!tIsInRange && tCritter->getEnt()->isVisible())
			{
				visibilityFlipped = true;
				tCritter->getObjectNode()->setVisible(false);
				if(tCritter->getIsDrawPoint())mEffectsManager->destroyParticleByObjectNode(tCritter->getObjectNode());
			}
			if(tIsInRange)
			{
				if(visibilityFlipped || tCritter->isPositionChanged(!tCritter->isAntiGravity()))updateGroundHeight(tCritter,true,true);
			}
			if(!tCritter->getIsDead() && !tCritter->getIsDrawPoint())
			{
				//Only update collisions for wild critters or my pet (or offline)
				if(!tCritter->getIsPet() || tCritter->imTheOwner() || mGameStateManager->isCampaign())updateCollision(tCritter,playerNode,tIsInRange);
				//Do attack
				if(tCritter->popIsReadyToAttack())
				{
					if(tIsInRange)
					{
						bool tHasAnim = false;
						for(unsigned char tAttackID = tCritter->getAttackID();(!tHasAnim && tAttackID>0);tAttackID--)
							tHasAnim = tCritter->setAnimation("Attack"+StringConverter::toString(tAttackID));
						const std::pair<CritterAttack,String> tAttackAndSound = mDef->getCritterAttackAndSound(tCritter->getCritterType(),tCritter->getAttackID()-1);
						const CritterAttack tAttack = tAttackAndSound.first;
						const Vector3 tOffset = Vector3(0,tCritter->getHeight()*0.25,tCritter->getLength()*0.5);
						const Real tHP = tAttack.hp + Math::RangeRandom(-0.05,0.05)*tAttack.hp;
						//Only create collisions for wild critters or my pet (or offline)
						if(!tCritter->getIsPet() || tCritter->imTheOwner() || mGameStateManager->isCampaign())
						mCollisionManager->createCollision(tCritter->getObjectNode(),tAttack.range,tAttack.hitForce,tOffset,tHP,tCritter->getAlliance(),tAttack.hitAlly);

						const String tSound = tAttackAndSound.second;
						if(tSound!="" && tCritter->getSoundTimer()==0)
						{
							mSoundManager->playSound(tSound.c_str(),tCritter->getObjectNode());
							tCritter->setSoundTimer(10);
						}

						//LOL
						if(tCritter->getCritterType()=="Earthshaker" && tCritter->getAttackID()==2)
						{
							weatherEffectRequest = "Earthquake2";
						}
					}
				}
			}
			if(tCritter->popAttackHasEnded())
			{
				mCollisionManager->destroyCollisionByOwnerNode(tCritter->getObjectNode());
				if(tCritter->imTheOwner() && !tCritter->getIsDead())tCritter->setDecisionTimer(1+Math::UnitRandom());
				tCritter->setTarget(0);
			}
			bool killedFlag = false;
			bool decomposedFlag = false;
			tCritter->update(evt,killedFlag,decomposedFlag);
			if(tCritter->imTheOwner() && !tCritter->getIsDrawPoint())
			{
				if((tCritter->getHP()>0 || tCritter->getIsPet()) && !tCritter->getIsDead())
				{
					if(tCritter->getDecisionTimer()==0)decisionQueue.push_back(tCritter);
					if(!tCritter->getHasAttack()&&tCritter->getAlliance()==ALLIANCE_ENEMY&&!tCritter->isAntiGravity()&&tCritter->getForce()==Vector3::ZERO)stationaryQueue.push_back(tCritter);
				}
				if(killedFlag)killedQueue.push_back(tCritter);
			}
			if(decomposedFlag)deadQueue.push_back(tCritter);
			boundCritterToMap(tCritter);
		}
	}
	bool isInRange(MagixCritter *critter)
	{
		if(mCamera)
		{
			const Real FARCLIP_SQUARED = Math::Sqr(mDef->viewDistance*2500 + 500);
			if(mCamera->getCamera()->isAttached())return(critter->getPosition().squaredDistance(mCamera->getCamera()->getDerivedPosition()) < FARCLIP_SQUARED);
			else return(critter->getPosition().squaredDistance(mCamera->getCamera()->getPosition()) < FARCLIP_SQUARED);
		}
		return false;
	}
	void boundCritterToMap(MagixCritter *critter)
	{
		const Vector2 tSize = mWorld->getWorldSize();
		const Vector2 tBounds = mWorld->getWorldBounds();
		const Real tMinX = tBounds.x;
		const Real tMinZ = tBounds.y;
		const Real tMaxX = tSize.x - tBounds.x;
		const Real tMaxZ = tSize.y - tBounds.y;

		if(critter->getPosition().x < tMinX)
		{
			critter->addPenaltyVelocity(Vector3(tMinX - critter->getPosition().x,0,0));
			critter->setTarget(critter->getPosition()+Vector3(500,0,0));
			critter->setDecisionTimer(8);
		}
		if(critter->getPosition().x > tMaxX)
		{
			critter->addPenaltyVelocity(Vector3(tMaxX - critter->getPosition().x,0,0));
			critter->setTarget(critter->getPosition()+Vector3(-500,0,0));
			critter->setDecisionTimer(8);
		}
		if(critter->getPosition().z < tMinZ)
		{
			critter->addPenaltyVelocity(Vector3(0,0,tMinZ - critter->getPosition().z));
			critter->setTarget(critter->getPosition()+Vector3(0,0,500));
			critter->setDecisionTimer(8);
		}
		if(critter->getPosition().z > tMaxZ)
		{
			critter->addPenaltyVelocity(Vector3(0,0,tMaxZ - critter->getPosition().z));
			critter->setTarget(critter->getPosition()+Vector3(0,0,-500));
			critter->setDecisionTimer(8);
		}
	}
	void updateCollision(MagixCritter *critter, SceneNode *playerNode, bool inRange=false)
	{
		const vector<Collision*>::type tHitList = mCollisionManager->getCollisionHitListForCritter(critter->getID(),critter->getAlliance(),critter->getEnt()->getWorldBoundingBox());
		for(vector<Collision*>::type::const_iterator it=tHitList.begin(); it!=tHitList.end(); it++)
		{
			Collision *coll = *it;
			if(!mGameStateManager->isCampaign())
				pushHitQueue(HitInfo(critter->getID(),coll->hp,(coll->getForce(false)==Vector3::ZERO?Vector3::ZERO:coll->getForce(true,critter->getPosition())),(critter->imTheOwner()||critter->getIsPet())));
			hitCritter(critter,coll->hp,coll->getForce(true,critter->getPosition()),0,inRange);
			if(coll->hp<0 && coll->mNode==playerNode)
			{
				mPlayerTarget = critter;
				if(critter->imTheOwner())critter->setLastHitIndex(0);
			}
		}
	}
	void hitCritter(MagixCritter *critter, const Real &damage, const Vector3 &force, const OwnerToken &token=0, bool inRange=false)
	{
		if(!critter)return;
		if(critter->getIsDead())return;
		critter->addHP(damage);
		if(damage<0)
		{
			if(!critter->isInvulnerable())
			{
				if(inRange||isInRange(critter))
				{
					mEffectsManager->createParticle(critter->getObjectNode(),"Hit1",0.25);
					mSoundManager->playSound(SOUND_HIT,critter->getObjectNode());
				}
				critter->addForce(force);
				if(token!=0 && critter->imTheOwner())critter->setLastHitIndex(token);
				if(critter->imTheOwner() && Math::UnitRandom()<0.5)critter->setDecisionTimer(critter->getDecisionTimer()*0.5);
			}
		}
		else if(damage>0)
		{
			if(inRange||isInRange(critter))
			{
				mEffectsManager->createParticle(critter->getObjectNode(),"HealHit1",0.25);
				mSoundManager->playSound(SOUND_HEALHIT,critter->getObjectNode());
			}
		}
	}
	void hitCritter(const unsigned short &iID, const Real &damage, const Vector3 &force, const OwnerToken &token)
	{
		hitCritter(getByID(iID),damage,force,token);
	}
	void updateGroundHeight(MagixCritter *critter, bool checkWaterCollision=false, bool checkCollBoxCollision=false)
	{
		Real tHeight = 0;
		static Ray updateRay;
		updateRay.setOrigin(critter->getPosition()+Vector3(0,5000,0));
		updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
		mRayQuery->setRay(updateRay);
		RaySceneQueryResult& qryResult = mRayQuery->execute();
		RaySceneQueryResult::iterator i = qryResult.begin();
		if(i != qryResult.end() && i->worldFragment)
		{
			tHeight = i->worldFragment->singleIntersection.y;
		}
		//Check water collision
		if(checkWaterCollision)
		{
			const Vector3 tVect = critter->getPosition();
			WaterBox *tWaterBox = mCollisionManager->getWaterBoxHit(tVect);
			if(tWaterBox && tWaterBox->isSolid && tHeight<tWaterBox->center.y)tHeight = tWaterBox->center.y;
		}
		//Check collbox collision
		if(checkCollBoxCollision)
		{
			const Vector3 tVect = critter->getPosition();
			const vector<CollBox*>::type tHitList = mCollisionManager->getCollBoxHitList(tVect,critter->getHeight());
			for(vector<CollBox*>::type::const_iterator it=tHitList.begin(); it!=tHitList.end(); it++)
			{
				CollBox *tBox = *it;
				const Real tMaxY = tBox->center.y+tBox->range.y;
				if(tHeight<tMaxY)tHeight = tMaxY;
			}
			const Vector3 tVect2 = critter->getPosition();
			const vector<CollSphere*>::type tHitList2 = mCollisionManager->getCollSphereHitList(tVect2);
			for(vector<CollSphere*>::type::const_iterator it=tHitList2.begin(); it!=tHitList2.end(); it++)
			{
				CollSphere *tSphere = *it;
				if(tSphere->range==0)continue;
				const Real tY = tSphere->center.y;
				const Real tX = tVect.x - tSphere->center.x;
				const Real tZ = tVect.z - tSphere->center.z;
				const Real tDist = Vector2(tX,tZ).length();
				if(tDist==0)
				{
					const Real tHeight2 = tY + tSphere->range;
					if(tHeight<tHeight2)tHeight = tHeight2;
				}
				else
				{
					const Radian tAngle = Math::ACos(tDist/tSphere->range);
					const Real tHeight2 = tY + Math::Sin(tAngle) * tSphere->range - GROUND_THRESHOLD;
					if(tHeight<tHeight2)tHeight = tHeight2;
				}
			}
		}
		critter->setGroundHeight(tHeight);
	}
	MagixCritter* createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const short &owner=-1)
	{
		deleteCritter(iID);
		Entity *ent = mSceneMgr->createEntity("Critter"+StringConverter::toString(iID),critter.mesh+".mesh");
		SceneNode *node = 0;
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		node->setPosition(position);
		node->setScale(critter.scale,critter.scale,critter.scale);
		ent->setQueryFlags(CRITTER_MASK);
		MagixCritter *tC = new MagixCritter(node,iID,critter.type,ent);
		tC->setAlliance(critter.friendly?ALLIANCE_FRIEND:ALLIANCE_ENEMY);
		tC->setHP(critter.hp);
		tC->setAntiGravity(critter.flying);
		tC->setOwnerIndex(owner);
		tC->setTarget(position);
		tC->setMaxSpeed(critter.maxSpeed);
		tC->setInvulnerable(critter.invulnerable);
		tC->setHasAttack(critter.attackList.size()>0);
		tC->setIsDrawPoint(critter.isDrawPoint);
		if(critter.isDrawPoint)mEffectsManager->createParticle(tC->getObjectNode(),"DrawPoint",-1);
		if(critter.material!="")tC->getEnt()->setMaterialName(critter.material);
		critterList.push_back(tC);
		if(tC->imTheOwner())myCritters.push_back(tC);
		critterCount++;

		return tC;
	}
	MagixCritter* createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const unsigned char &worldID, const short &owner=-1)
	{
		MagixCritter *tC = createCritter(iID,critter,position,owner);
		tC->setWorldCritterID(worldID);
		return tC;
	}
	void deleteCritter(MagixCritter *tCritter, const vector<MagixCritter*>::type::iterator &it)
	{
		if(!tCritter)return;
		if(!tCritter->getIsPet())critterCount--;
		removeChaseTarget(tCritter);
		removeFromMyCritters(tCritter);
		for(vector<MagixCritter*>::type::iterator i=stationaryQueue.begin();i!=stationaryQueue.end();i++)
			if(*i==tCritter)
			{
				stationaryQueue.erase(i);
				break;
			}
		for(vector<MagixCritter*>::type::iterator i=decisionQueue.begin();i!=decisionQueue.end();i++)
			if(*i==tCritter)
			{
				decisionQueue.erase(i);
				break;
			}
		for(vector<MagixCritter*>::type::iterator i=killedQueue.begin();i!=killedQueue.end();i++)
			if(*i==tCritter)
			{
				killedQueue.erase(i);
				break;
			}
		for(vector<MagixCritter*>::type::iterator i=deadQueue.begin();i!=deadQueue.end();i++)
			if(*i==tCritter)
			{
				deadQueue.erase(i);
				break;
			}
		SceneNode *tObjectNode = tCritter->getObjectNode();
		if(tObjectNode)
		{
			//if(tCritter->getEnt())mEffectsManager->destroyRibbonTrailByEntity(tCritter->getEnt());
			mEffectsManager->destroyParticleByObjectNode(tObjectNode);
			mCollisionManager->destroyCollisionByOwnerNode(tObjectNode);
			mSoundManager->clearChannelsConnectedToSceneNode(tObjectNode);
			tObjectNode->detachAllObjects();
			mSceneMgr->destroySceneNode(tObjectNode->getName());
		}
		if(tCritter->getEnt())mSceneMgr->destroyEntity(tCritter->getEnt());
		critterList.erase(it);
		delete tCritter;
	}
	void deleteCritter(const unsigned short &iID)
	{
		for(vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
		{
			MagixCritter *tCritter = *it;
			if(tCritter->getID()==iID)
			{
				deleteCritter(tCritter,it);
				return;
			}
		}
	}
	void deleteAllCritters()
	{
		while(critterList.size()>0)
		{
			vector<MagixCritter*>::type::iterator it = critterList.end();
			it--;
			MagixCritter *tCritter = *it;
			deleteCritter(tCritter,it);
		}
	}
	MagixCritter* getByObjectNode(SceneNode *node)
	{
		for(vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
		{
			MagixCritter *tCritter = *it;
			if(tCritter->getObjectNode()==node)
			{
				return tCritter;
			}
		}
		return 0;
	}
	MagixCritter* getByID(const unsigned short &iID)
	{
		for(vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
		{
			MagixCritter *tCritter = *it;
			if(tCritter->getID()==iID)
			{
				return tCritter;
			}
		}
		return 0;
	}
	const unsigned short getNumCritters(bool includePets=false)
	{
		return (includePets?(unsigned short)critterList.size():critterCount);
	}
	void decrementCritterCount()
	{
		critterCount--;
	}
	const unsigned short getNextEmptyID(const unsigned short &start)
	{
		unsigned short tID = start;
		bool tFound = false;
		while(!tFound)
		{
			tFound = true;
			for(vector<const unsigned short>::type::iterator it = sentID.begin(); it != sentID.end(); it++)
			{
				if(*it==tID)
				{
					tFound = false;
					break;
				}
			}
			if(tFound)
			{
				for(vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
				{
					MagixCritter *tCritter = *it;
					if(tCritter->getID()==tID)
					{
						tFound = false;
						break;
					}
				}
			}
			if(tFound)break;
			else tID++;
		}
		return tID;
	}
	const vector<MagixCritter*>::type getCritterList()
	{
		return critterList;
	}
	const vector<MagixCritter*>::type popKilledQueue()
	{
		const vector<MagixCritter*>::type tList = killedQueue;
		killedQueue.clear();
		return tList;
	}
	const vector<MagixCritter*>::type popDeadQueue()
	{
		const vector<MagixCritter*>::type tList = deadQueue;
		deadQueue.clear();
		return tList;
	}
	const vector<const HitInfo>::type popHitQueue()
	{
		const vector<const HitInfo>::type tList = hitQueue;
		hitQueue.clear();
		return tList;
	}
	void pushHitQueue(const HitInfo &info)
	{
		hitQueue.push_back(info);
	}
	const vector<MagixCritter*>::type popDecisionQueue()
	{
		const vector<MagixCritter*>::type tList = decisionQueue;
		decisionQueue.clear();
		return tList;
	}
	const vector<MagixCritter*>::type popStationaryQueue()
	{
		const vector<MagixCritter*>::type tList = stationaryQueue;
		stationaryQueue.clear();
		return tList;
	}
	const vector<MagixCritter*>::type getMyCritters()
	{
		return myCritters;
	}
	void pushMyCritters(MagixCritter *critter)
	{
		myCritters.push_back(critter);
	}
	void removeFromMyCritters(MagixCritter *critter)
	{
		for(vector<MagixCritter*>::type::iterator i=myCritters.begin();i!=myCritters.end();i++)
			if(*i==critter)
			{
				myCritters.erase(i);
				break;
			}
	}
	MagixLiving* popPlayerTarget()
	{
		MagixLiving *tTarget = mPlayerTarget;
		mPlayerTarget = 0;
		return tTarget;
	}
	void removeChaseTarget(MagixObject *target)
	{
		for(int i=0;i<(int)critterList.size();i++)
		{
			if(critterList[i]->getChaseTarget()==target)
			{
				critterList[i]->setTarget(0);
				if(critterList[i]->imTheOwner())critterList[i]->setDecisionTimer(0);
			}
		}
	}
	void pushSentID(const unsigned short &iID)
	{
		sentID.push_back(iID);
	}
	void popSentID(const unsigned short &iID)
	{
		for(vector<const unsigned short>::type::iterator it=sentID.begin();it!=sentID.end();it++)
		{
			if(*it==iID)
			{
				sentID.erase(it);
				return;
			}
		}
	}
	void setIsPet(MagixCritter *critter)
	{
		if(!critter)return;
		critter->setIsPet(true);
		if(critter->getObjectNode()->getScale().y>1.5)
		{
			critter->getObjectNode()->setScale(1.5,1.5,1.5);
			critter->resetSize();
		}
		critterCount--;
	}
	const String popWeatherEffectRequest()
	{
		const String tEffect = weatherEffectRequest;
		weatherEffectRequest = "";
		return tEffect;
	}
};

#endif