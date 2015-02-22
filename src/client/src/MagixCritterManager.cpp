#include "MagixCritterManager.h"
#include "MagixConst.h"

void MagixCritterManager::initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixEffectsManager *effectsMgr, MagixCollisionManager *collMgr, MagixSoundManager *soundMgr, MagixWorld *world, MagixCamera *camera)
{
	mSceneMgr = sceneMgr;
	mDef = def;
	mGameStateManager = gameStateMgr;
	mEffectsManager = effectsMgr;
	mCollisionManager = collMgr;
	mSoundManager = soundMgr;
	mWorld = world;
	mCamera = camera;
	mRayQuery = sceneMgr->createRayQuery(Ray());
}
void MagixCritterManager::reset()
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
void MagixCritterManager::update(const FrameEvent &evt, SceneNode *playerNode)
{
	for (vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
	{
		MagixCritter *tCritter = *it;
		//Range check
		bool tIsInRange = isInRange(tCritter);
		bool visibilityFlipped = false;
		if (tIsInRange && !tCritter->getEnt()->isVisible())
		{
			visibilityFlipped = true;
			tCritter->getObjectNode()->setVisible(true);
			if (tCritter->getIsDrawPoint())mEffectsManager->createParticle(tCritter->getObjectNode(), "DrawPoint", -1);
		}
		else if (!tIsInRange && tCritter->getEnt()->isVisible())
		{
			visibilityFlipped = true;
			tCritter->getObjectNode()->setVisible(false);
			if (tCritter->getIsDrawPoint())mEffectsManager->destroyParticleByObjectNode(tCritter->getObjectNode());
		}
		if (tIsInRange)
		{
			if (visibilityFlipped || tCritter->isPositionChanged(!tCritter->isAntiGravity()))updateGroundHeight(tCritter, true, true);
		}
		if (!tCritter->getIsDead() && !tCritter->getIsDrawPoint())
		{
			//Only update collisions for wild critters or my pet (or offline)
			if (!tCritter->getIsPet() || tCritter->imTheOwner() || mGameStateManager->isCampaign())updateCollision(tCritter, playerNode, tIsInRange);
			//Do attack
			if (tCritter->popIsReadyToAttack())
			{
				if (tIsInRange)
				{
					bool tHasAnim = false;
					for (unsigned char tAttackID = tCritter->getAttackID(); (!tHasAnim && tAttackID>0); tAttackID--)
						tHasAnim = tCritter->setAnimation("Attack" + StringConverter::toString(tAttackID));
					const std::pair<CritterAttack, String> tAttackAndSound = mDef->getCritterAttackAndSound(tCritter->getCritterType(), tCritter->getAttackID() - 1);
					const CritterAttack tAttack = tAttackAndSound.first;
					const Vector3 tOffset = Vector3(0, (Real)(tCritter->getHeight()*0.25), (Real)(tCritter->getLength()*0.5));
					const Real tHP = tAttack.hp + Math::RangeRandom(-0.05, 0.05)*tAttack.hp;
					//Only create collisions for wild critters or my pet (or offline)
					if (!tCritter->getIsPet() || tCritter->imTheOwner() || mGameStateManager->isCampaign())
						mCollisionManager->createCollision(tCritter->getObjectNode(), tAttack.range, tAttack.hitForce, tOffset, (short)tHP, tCritter->getAlliance(), tAttack.hitAlly);

					const String tSound = tAttackAndSound.second;
					if (tSound != "" && tCritter->getSoundTimer() == 0)
					{
						mSoundManager->playSound(tSound.c_str(), tCritter->getObjectNode());
						tCritter->setSoundTimer(10);
					}

					//LOL
					if (tCritter->getCritterType() == "Earthshaker" && tCritter->getAttackID() == 2)
					{
						weatherEffectRequest = "Earthquake2";
					}
				}
			}
		}
		if (tCritter->popAttackHasEnded())
		{
			mCollisionManager->destroyCollisionByOwnerNode(tCritter->getObjectNode());
			if (tCritter->imTheOwner() && !tCritter->getIsDead())tCritter->setDecisionTimer(1 + Math::UnitRandom());
			tCritter->setTarget(0);
		}
		bool killedFlag = false;
		bool decomposedFlag = false;
		tCritter->update(evt, killedFlag, decomposedFlag);
		if (tCritter->imTheOwner() && !tCritter->getIsDrawPoint())
		{
			if ((tCritter->getHP()>0 || tCritter->getIsPet()) && !tCritter->getIsDead())
			{
				if (tCritter->getDecisionTimer() == 0)decisionQueue.push_back(tCritter);
				if (!tCritter->getHasAttack() && tCritter->getAlliance() == ALLIANCE_ENEMY&&!tCritter->isAntiGravity() && tCritter->getForce() == Vector3::ZERO)stationaryQueue.push_back(tCritter);
			}
			if (killedFlag)killedQueue.push_back(tCritter);
		}
		if (decomposedFlag)deadQueue.push_back(tCritter);
		boundCritterToMap(tCritter);
	}
}
bool MagixCritterManager::isInRange(MagixCritter *critter)
{
	if (mCamera)
	{
		const Real FARCLIP_SQUARED = Math::Sqr(mDef->viewDistance * 2500 + 500);
		if (mCamera->getCamera()->isAttached())return(critter->getPosition().squaredDistance(mCamera->getCamera()->getDerivedPosition()) < FARCLIP_SQUARED);
		else return(critter->getPosition().squaredDistance(mCamera->getCamera()->getPosition()) < FARCLIP_SQUARED);
	}
	return false;
}
void MagixCritterManager::boundCritterToMap(MagixCritter *critter)
{
	const Vector2 tSize = mWorld->getWorldSize();
	const Vector2 tBounds = mWorld->getWorldBounds();
	const Real tMinX = tBounds.x;
	const Real tMinZ = tBounds.y;
	const Real tMaxX = tSize.x - tBounds.x;
	const Real tMaxZ = tSize.y - tBounds.y;

	if (critter->getPosition().x < tMinX)
	{
		critter->addPenaltyVelocity(Vector3(tMinX - critter->getPosition().x, 0, 0));
		critter->setTarget(critter->getPosition() + Vector3(500, 0, 0));
		critter->setDecisionTimer(8);
	}
	if (critter->getPosition().x > tMaxX)
	{
		critter->addPenaltyVelocity(Vector3(tMaxX - critter->getPosition().x, 0, 0));
		critter->setTarget(critter->getPosition() + Vector3(-500, 0, 0));
		critter->setDecisionTimer(8);
	}
	if (critter->getPosition().z < tMinZ)
	{
		critter->addPenaltyVelocity(Vector3(0, 0, tMinZ - critter->getPosition().z));
		critter->setTarget(critter->getPosition() + Vector3(0, 0, 500));
		critter->setDecisionTimer(8);
	}
	if (critter->getPosition().z > tMaxZ)
	{
		critter->addPenaltyVelocity(Vector3(0, 0, tMaxZ - critter->getPosition().z));
		critter->setTarget(critter->getPosition() + Vector3(0, 0, -500));
		critter->setDecisionTimer(8);
	}
}
void MagixCritterManager::updateCollision(MagixCritter *critter, SceneNode *playerNode, bool inRange)
{
	const vector<Collision*>::type tHitList = mCollisionManager->getCollisionHitListForCritter(critter->getID(), critter->getAlliance(), critter->getEnt()->getWorldBoundingBox());
	for (vector<Collision*>::type::const_iterator it = tHitList.begin(); it != tHitList.end(); it++)
	{
		Collision *coll = *it;
		if (!mGameStateManager->isCampaign())
			pushHitQueue(HitInfo(critter->getID(), coll->hp, (coll->getForce(false) == Vector3::ZERO ? Vector3::ZERO : coll->getForce(true, critter->getPosition())), (critter->imTheOwner() || critter->getIsPet())));
		hitCritter(critter, coll->hp, coll->getForce(true, critter->getPosition()), 0, inRange);
		if (coll->hp<0 && coll->mNode == playerNode)
		{
			mPlayerTarget = critter;
			if (critter->imTheOwner())critter->setLastHitIndex(0);
		}
	}
}
void MagixCritterManager::hitCritter(MagixCritter *critter, const Real &damage, const Vector3 &force, const OwnerToken &token, bool inRange)
{
	if (!critter)return;
	if (critter->getIsDead())return;
	critter->addHP((short)damage);
	if (damage<0)
	{
		if (!critter->isInvulnerable())
		{
			if (inRange || isInRange(critter))
			{
				mEffectsManager->createParticle(critter->getObjectNode(), "Hit1", 0.25);
				mSoundManager->playSound(SOUND_HIT, critter->getObjectNode());
			}
			critter->addForce(force);
			if (token != 0 && critter->imTheOwner())critter->setLastHitIndex(token);
			if (critter->imTheOwner() && Math::UnitRandom()<0.5)critter->setDecisionTimer((Real)(critter->getDecisionTimer()*0.5));
		}
	}
	else if (damage>0)
	{
		if (inRange || isInRange(critter))
		{
			mEffectsManager->createParticle(critter->getObjectNode(), "HealHit1", 0.25);
			mSoundManager->playSound(SOUND_HEALHIT, critter->getObjectNode());
		}
	}
}
void MagixCritterManager::hitCritter(const unsigned short &iID, const Real &damage, const Vector3 &force, const OwnerToken &token)
{
	hitCritter(getByID(iID), damage, force, token);
}
void MagixCritterManager::updateGroundHeight(MagixCritter *critter, bool checkWaterCollision, bool checkCollBoxCollision)
{
	Real tHeight = 0;
	static Ray updateRay;
	updateRay.setOrigin(critter->getPosition() + Vector3(0, 5000, 0));
	updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
	mRayQuery->setRay(updateRay);
	RaySceneQueryResult& qryResult = mRayQuery->execute();
	RaySceneQueryResult::iterator i = qryResult.begin();
	if (i != qryResult.end() && i->worldFragment)
	{
		tHeight = i->worldFragment->singleIntersection.y;
	}
	//Check water collision
	if (checkWaterCollision)
	{
		const Vector3 tVect = critter->getPosition();
		WaterBox *tWaterBox = mCollisionManager->getWaterBoxHit(tVect);
		if (tWaterBox && tWaterBox->isSolid && tHeight<tWaterBox->center.y)tHeight = tWaterBox->center.y;
	}
	//Check collbox collision
	if (checkCollBoxCollision)
	{
		const Vector3 tVect = critter->getPosition();
		const vector<CollBox*>::type tHitList = mCollisionManager->getCollBoxHitList(tVect, critter->getHeight());
		for (vector<CollBox*>::type::const_iterator it = tHitList.begin(); it != tHitList.end(); it++)
		{
			CollBox *tBox = *it;
			const Real tMaxY = tBox->center.y + tBox->range.y;
			if (tHeight<tMaxY)tHeight = tMaxY;
		}
		const Vector3 tVect2 = critter->getPosition();
		const vector<CollSphere*>::type tHitList2 = mCollisionManager->getCollSphereHitList(tVect2);
		for (vector<CollSphere*>::type::const_iterator it = tHitList2.begin(); it != tHitList2.end(); it++)
		{
			CollSphere *tSphere = *it;
			if (tSphere->range == 0)continue;
			const Real tY = tSphere->center.y;
			const Real tX = tVect.x - tSphere->center.x;
			const Real tZ = tVect.z - tSphere->center.z;
			const Real tDist = Vector2(tX, tZ).length();
			if (tDist == 0)
			{
				const Real tHeight2 = tY + tSphere->range;
				if (tHeight<tHeight2)tHeight = tHeight2;
			}
			else
			{
				const Radian tAngle = Math::ACos(tDist / tSphere->range);
				const Real tHeight2 = tY + Math::Sin(tAngle) * tSphere->range - GROUND_THRESHOLD;
				if (tHeight<tHeight2)tHeight = tHeight2;
			}
		}
	}
	critter->setGroundHeight(tHeight);
}
MagixCritter* MagixCritterManager::createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const short &owner)
{
	deleteCritter(iID);
	Entity *ent = mSceneMgr->createEntity("Critter" + StringConverter::toString(iID), critter.mesh + ".mesh");
	SceneNode *node = 0;
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(position);
	node->setScale(critter.scale, critter.scale, critter.scale);
	ent->setQueryFlags(CRITTER_MASK);
	MagixCritter *tC = new MagixCritter(node, iID, critter.type, ent);
	tC->setAlliance(critter.friendly ? ALLIANCE_FRIEND : ALLIANCE_ENEMY);
	tC->setHP((short)(critter.hp));
	tC->setAntiGravity(critter.flying);
	tC->setOwnerIndex(owner);
	tC->setTarget(position);
	tC->setMaxSpeed(critter.maxSpeed);
	tC->setInvulnerable(critter.invulnerable);
	tC->setHasAttack(critter.attackList.size()>0);
	tC->setIsDrawPoint(critter.isDrawPoint);
	if (critter.isDrawPoint)mEffectsManager->createParticle(tC->getObjectNode(), "DrawPoint", -1);
	if (critter.material != "")tC->getEnt()->setMaterialName(critter.material);
	critterList.push_back(tC);
	if (tC->imTheOwner())myCritters.push_back(tC);
	critterCount++;

	return tC;
}
MagixCritter* MagixCritterManager::createCritter(const unsigned short &iID, const Critter &critter, const Vector3 &position, const unsigned char &worldID, const short &owner)
{
	MagixCritter *tC = createCritter(iID, critter, position, owner);
	tC->setWorldCritterID(worldID);
	return tC;
}
void MagixCritterManager::deleteCritter(MagixCritter *tCritter, const vector<MagixCritter*>::type::iterator &it)
{
	if (!tCritter)return;
	if (!tCritter->getIsPet())critterCount--;
	removeChaseTarget(tCritter);
	removeFromMyCritters(tCritter);
	for (vector<MagixCritter*>::type::iterator i = stationaryQueue.begin(); i != stationaryQueue.end(); i++)
		if (*i == tCritter)
		{
			stationaryQueue.erase(i);
			break;
		}
	for (vector<MagixCritter*>::type::iterator i = decisionQueue.begin(); i != decisionQueue.end(); i++)
		if (*i == tCritter)
		{
			decisionQueue.erase(i);
			break;
		}
	for (vector<MagixCritter*>::type::iterator i = killedQueue.begin(); i != killedQueue.end(); i++)
		if (*i == tCritter)
		{
			killedQueue.erase(i);
			break;
		}
	for (vector<MagixCritter*>::type::iterator i = deadQueue.begin(); i != deadQueue.end(); i++)
		if (*i == tCritter)
		{
			deadQueue.erase(i);
			break;
		}
	SceneNode *tObjectNode = tCritter->getObjectNode();
	if (tObjectNode)
	{
		//if(tCritter->getEnt())mEffectsManager->destroyRibbonTrailByEntity(tCritter->getEnt());
		mEffectsManager->destroyParticleByObjectNode(tObjectNode);
		mCollisionManager->destroyCollisionByOwnerNode(tObjectNode);
		mSoundManager->clearChannelsConnectedToSceneNode(tObjectNode);
		tObjectNode->detachAllObjects();
		mSceneMgr->destroySceneNode(tObjectNode->getName());
	}
	if (tCritter->getEnt())mSceneMgr->destroyEntity(tCritter->getEnt());
	critterList.erase(it);
	delete tCritter;
}
void MagixCritterManager::deleteCritter(const unsigned short &iID)
{
	for (vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
	{
		MagixCritter *tCritter = *it;
		if (tCritter->getID() == iID)
		{
			deleteCritter(tCritter, it);
			return;
		}
	}
}
void MagixCritterManager::deleteAllCritters()
{
	while (critterList.size()>0)
	{
		vector<MagixCritter*>::type::iterator it = critterList.end();
		it--;
		MagixCritter *tCritter = *it;
		deleteCritter(tCritter, it);
	}
}
MagixCritter* MagixCritterManager::getByObjectNode(SceneNode *node)
{
	for (vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
	{
		MagixCritter *tCritter = *it;
		if (tCritter->getObjectNode() == node)
		{
			return tCritter;
		}
	}
	return 0;
}
MagixCritter* MagixCritterManager::getByID(const unsigned short &iID)
{
	for (vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
	{
		MagixCritter *tCritter = *it;
		if (tCritter->getID() == iID)
		{
			return tCritter;
		}
	}
	return 0;
}
const unsigned short MagixCritterManager::getNumCritters(bool includePets)
{
	return (includePets ? (unsigned short)critterList.size() : critterCount);
}
void MagixCritterManager::decrementCritterCount()
{
	critterCount--;
}
const unsigned short MagixCritterManager::getNextEmptyID(const unsigned short &start)
{
	unsigned short tID = start;
	bool tFound = false;
	while (!tFound)
	{
		tFound = true;
		for (vector<const unsigned short>::type::iterator it = sentID.begin(); it != sentID.end(); it++)
		{
			if (*it == tID)
			{
				tFound = false;
				break;
			}
		}
		if (tFound)
		{
			for (vector<MagixCritter*>::type::iterator it = critterList.begin(); it != critterList.end(); it++)
			{
				MagixCritter *tCritter = *it;
				if (tCritter->getID() == tID)
				{
					tFound = false;
					break;
				}
			}
		}
		if (tFound)break;
		else tID++;
	}
	return tID;
}
const vector<MagixCritter*>::type MagixCritterManager::getCritterList()
{
	return critterList;
}
const vector<MagixCritter*>::type MagixCritterManager::popKilledQueue()
{
	const vector<MagixCritter*>::type tList = killedQueue;
	killedQueue.clear();
	return tList;
}
const vector<MagixCritter*>::type MagixCritterManager::popDeadQueue()
{
	const vector<MagixCritter*>::type tList = deadQueue;
	deadQueue.clear();
	return tList;
}
const vector<const HitInfo>::type MagixCritterManager::popHitQueue()
{
	const vector<const HitInfo>::type tList = hitQueue;
	hitQueue.clear();
	return tList;
}
void MagixCritterManager::pushHitQueue(const HitInfo &info)
{
	hitQueue.push_back(info);
}
const vector<MagixCritter*>::type MagixCritterManager::popDecisionQueue()
{
	const vector<MagixCritter*>::type tList = decisionQueue;
	decisionQueue.clear();
	return tList;
}
const vector<MagixCritter*>::type MagixCritterManager::popStationaryQueue()
{
	const vector<MagixCritter*>::type tList = stationaryQueue;
	stationaryQueue.clear();
	return tList;
}
const vector<MagixCritter*>::type MagixCritterManager::getMyCritters()
{
	return myCritters;
}
void MagixCritterManager::pushMyCritters(MagixCritter *critter)
{
	myCritters.push_back(critter);
}
void MagixCritterManager::removeFromMyCritters(MagixCritter *critter)
{
	for (vector<MagixCritter*>::type::iterator i = myCritters.begin(); i != myCritters.end(); i++)
		if (*i == critter)
		{
			myCritters.erase(i);
			break;
		}
}
MagixLiving* MagixCritterManager::popPlayerTarget()
{
	MagixLiving *tTarget = mPlayerTarget;
	mPlayerTarget = 0;
	return tTarget;
}
void MagixCritterManager::removeChaseTarget(MagixObject *target)
{
	for (int i = 0; i<(int)critterList.size(); i++)
	{
		if (critterList[i]->getChaseTarget() == target)
		{
			critterList[i]->setTarget(0);
			if (critterList[i]->imTheOwner())critterList[i]->setDecisionTimer(0);
		}
	}
}
void MagixCritterManager::pushSentID(const unsigned short &iID)
{
	sentID.push_back(iID);
}
void MagixCritterManager::popSentID(const unsigned short &iID)
{
	for (vector<const unsigned short>::type::iterator it = sentID.begin(); it != sentID.end(); it++)
	{
		if (*it == iID)
		{
			sentID.erase(it);
			return;
		}
	}
}
void MagixCritterManager::setIsPet(MagixCritter *critter)
{
	if (!critter)return;
	critter->setIsPet(true);
	if (critter->getObjectNode()->getScale().y>1.5)
	{
		critter->getObjectNode()->setScale(1.5, 1.5, 1.5);
		critter->resetSize();
	}
	critterCount--;
}
const String MagixCritterManager::popWeatherEffectRequest()
{
	const String tEffect = weatherEffectRequest;
	weatherEffectRequest = "";
	return tEffect;
}