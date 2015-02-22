#include "MagixUnitManager.h"

MagixUnitManager::MagixUnitManager()
{
	mPlayer = new MagixPlayer;
	mDef = 0;
	mGameStateManager = 0;
	mLastUnit = 0;
	mWorld = 0;
	mEffectsManager = 0;
	mCollisionManager = 0;
	mSoundManager = 0;
	mItemManager = 0;
	mCritterManager = 0;
	mCamera = 0;
	mSceneMgr = 0;
	mRayQuery = 0;
	mNameTagAttr = 0;
	mUserTagAttr = 0;
	mChatBubbleAttr = 0;
	mPlayerTarget = 0;
	playerTargetChanged = false;
	shouldNameTagsBeVisible = false;
	itemDropQueue.clear();
	pickupText = "";
	playerHasNewAttack = false;
	partyMembers.clear();
	partyChanged = false;
	partyInviter = 0;
	hitQueue.clear();
	tameFlag = TameData();
}
MagixUnitManager::~MagixUnitManager()
{
	deleteAllUnits();
	destroyUnit(mPlayer);
	delete mPlayer;
	if (mRayQuery)delete mRayQuery;
	if (mNameTagAttr)delete mNameTagAttr;
	if (mUserTagAttr)delete mUserTagAttr;
	if (mChatBubbleAttr)delete mChatBubbleAttr;
}
void MagixUnitManager::initialize(SceneManager *sceneMgr, MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr, MagixWorld *world, MagixEffectsManager *effectsMgr, MagixCollisionManager *collisionMgr, MagixSoundManager *soundMgr, MagixItemManager *itemMgr, MagixCritterManager *critterMgr, MagixCamera *camera)
{
	mSceneMgr = sceneMgr;
	mDef = def;
	mGameStateManager = gameStateMgr;
	mWorld = world;
	mEffectsManager = effectsMgr;
	mCollisionManager = collisionMgr;
	mSoundManager = soundMgr;
	mItemManager = itemMgr;
	mCritterManager = critterMgr;
	mCamera = camera;
	mRayQuery = sceneMgr->createRayQuery(Ray());
	mNameTagAttr = new MovableTextOverlayAttributes("Attrs1", sceneMgr->getCamera("PlayerCam"), "Tahoma", 16, ColourValue::White, "GUIMat/ButtonUp");
	mChatBubbleAttr = new MovableTextOverlayAttributes("Attrs2", sceneMgr->getCamera("PlayerCam"), "Tahoma", 16, ColourValue(1, 0.6, 0), "GUIMat/StatsBlockCenter");
	mUserTagAttr = new MovableTextOverlayAttributes("Attrs3", sceneMgr->getCamera("PlayerCam"), "Tahoma", 14, ColourValue(1, 0.6, 0), "GUIMat/ButtonUp");
}
void MagixUnitManager::reset(bool destroyPlayer, bool resetPosition)
{
	mapChange = MapChangeData();
	mPlayerTarget = 0;
	playerTargetChanged = false;
	deleteAllUnits();
	if (destroyPlayer)
	{
		mPlayer->reset();
		destroyUnit(mPlayer);
		clearPartyMembers();
	}
	else
	{
		if (mPlayer->getAutoTrackObject())mPlayer->lockOn(0);
		if (mPlayer->getAutoAttackTarget())mPlayer->setAutoAttack(0);
		if (resetPosition)resetPlayerPosition();
		if (mPlayer->getPetFlags()->attackTarget)mPlayer->getPetFlags()->attackTarget = 0;
	}
	hitQueue.clear();
}
void MagixUnitManager::resetPlayerPosition()
{
	if (!mPlayer->getObjectNode())return;
	Vector3 tVect = mWorld->getSpawnSquare();
	mPlayer->setPosition(tVect.x + Math::RangeRandom(0, tVect.z), 700, tVect.y + Math::RangeRandom(0, tVect.z));
	mPlayer->getObjectNode()->setOrientation(Quaternion(Degree(0), Vector3::UNIT_Y));
	mPlayer->resetTarget();
	clampUnitToTerrain(mPlayer, true);
}
void MagixUnitManager::reclampAllUnitsToTerrain(bool clampToTerrain)
{
	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		clampUnitToTerrain(tLast, clampToTerrain);
		tLast = tLast->getPrevious();
	}
}
#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
void MagixUnitManager::update(const FrameEvent &evt)
{
	/*RectLayoutManager nameTagRect(0,0,mSceneMgr->getCamera("PlayerCam")->getViewport()->getActualWidth(),mSceneMgr->getCamera("PlayerCam")->getViewport()->getActualHeight());
	nameTagRect.setDepth(0);
	RectLayoutManager chatBubbleRect(0,0,mSceneMgr->getCamera("PlayerCam")->getViewport()->getActualWidth(),mSceneMgr->getCamera("PlayerCam")->getViewport()->getActualHeight());
	chatBubbleRect.setDepth(0);*/

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		if (tLast != static_cast<MagixUnit*>(mPlayer))
		{
//			DBOUT("1 MagixUnitManager::update" << std::endl);
			updateUnit(tLast, evt, false);
		}
		tLast = tLast->getPrevious();
	}

	//Player routine update
//	DBOUT("2 MagixUnitManager::update" << std::endl);
	updateUnit(mPlayer, evt, true);
	updateWallCollisions(mPlayer);
	boundUnitToMap(mPlayer);
	updatePortalCollisions(mPlayer);
	updateGateCollisions(mPlayer);
}
void MagixUnitManager::updateUnit(MagixUnit *unit, const FrameEvent &evt, bool isPlayer)
{
	const Quaternion camOrientation = (mCamera ? mCamera->getCameraOrientation() : Quaternion::ZERO);
	bool tIsInRange = !unit->isHidden; //Hidden = out of range
//	DBOUT("MagixUnitManager::updateUnit" << std::endl);
	//Routine update
	if (isPlayer)
	{
		static_cast<MagixPlayer*>(unit)->update(evt, mDef, camOrientation);
		if (unit->isPositionChanged(!unit->isAntiGravity()))unit->allocFlags.hasOwnGroundHeight = false;
	}
	else unit->update(evt, mDef);
	if ((isPlayer || !unit->allocFlags.hasOwnGroundHeight) && unit->isPositionChanged(!unit->isAntiGravity()))clampUnitToTerrain(unit);
	if (mWorld->getIsInterior() && unit->isPositionChanged())clampUnitToCeiling(unit);

	//Range check
	if (tIsInRange)tIsInRange = isInRange(unit);
	if (!tIsInRange)
	{
		if (unit->getBodyEnt()->isVisible())unit->getObjectNode()->setVisible(false);
		unit->showChatBubble(false);
		unit->showNameTag(false);
		unit->showUserTag(false);
	}
	else
	{
		if (!unit->getBodyEnt()->isVisible())unit->getObjectNode()->setVisible(true);
		unit->showChatBubble(true);
		if (shouldNameTagsBeVisible && !unit->isNameTagVisible())
		{
			unit->showNameTag(true);
			unit->updateNameTag(evt);
		}
		if (shouldNameTagsBeVisible && !unit->isUserTagVisible())
		{
			unit->showUserTag(true);
			unit->updateUserTag(evt);
		}
	}

	if (tIsInRange)
	{
		if (isPlayer)updateCollBoxCollisions(unit, evt);
		updateWaterCollisions(unit);
		updateMapEffects(unit);
		updateSounds(unit);
	}

	if (unit->isNameTagVisible())updateNameTag(/*nameTagRect,*/unit->getNameTag());
	if (unit->isUserTagVisible())updateNameTag(/*nameTagRect,*/unit->getUserTag());
	if (unit->isChatBubbleVisible())updateChatBubble(/*chatBubbleRect,*/unit->getChatBubble());
	if (unit->popHasStoppedAttack())
	{
		mEffectsManager->destroyRibbonTrailByEntity(unit->getBodyEnt());
		if (unit->getWingEnt())mEffectsManager->destroyRibbonTrailByEntity(unit->getWingEnt());
		mCollisionManager->destroyCollisionByOwnerNode(unit->getObjectNode());
		unit->setSpeedMultiplier(1);
	}
	if (unit->isDying())return;

	if (unit->popHasNewAttack())
	{
		const String tAttackName = unit->getAttackName();
		const Attack tAttack = mDef->getAttack(tAttackName);
		if (tAttack.name != "")
		{
			if (tIsInRange)
			{
				for (int i = 0; i<(int)tAttack.FX.size(); i++)
				{
					const AttackFX tFX = tAttack.FX[i];
					Entity *tEnt = unit->getBodyEnt();
					if (StringUtil::startsWith(tFX.bone, "wing"))tEnt = unit->getWingEnt();
					else if (StringUtil::startsWith(tFX.bone, "tail"))tEnt = unit->getTailEnt();
					else if (StringUtil::startsWith(tFX.bone, "head"))tEnt = unit->getHeadEnt();
					if (tEnt)mEffectsManager->createRibbonTrail(tEnt, tFX.bone, tFX.trailMat, tFX.trailWidth, tFX.colour, tFX.colourChange, tFX.trailHeadMat);
				}
			}
			if (tAttack.singleTarget)
			{
				if (isPlayer || mGameStateManager->isCampaign())
				{
					const Real tHP = Math::RangeRandom(tAttack.hpMin, tAttack.hpMax);
					MagixObject *target = 0;
					//Attacker is player
					if (isPlayer)
					{
						//Set target to be locked on target for CAMERA_LOCKED
						if (mCamera->getCameraMode() == CAMERA_LOCKED)
						{
							target = mPlayerTarget;
							if (!target && mPlayer->getIsLockedOn())target = unit->getAutoTrackObject();
						}
						//Set target to be autoattack target otherwise
						else target = static_cast<MagixPlayer*>(unit)->getAutoAttackTarget();
					}
					//Not player, target to be tracked object
					else target = unit->getAutoTrackObject();

					//Set target to be self if alliance doesn't match
					if (target)
					{
						if (target->getType() == OBJECT_CRITTER || target->getType() == OBJECT_UNIT)
						{
							bool tIsAlly = static_cast<MagixLiving*>(target)->matchAlliance(unit->getAlliance());
							if (tAttack.hitAlly && !tIsAlly)target = unit;
						}
					}
					//Set target to be self if there's no target
					else if (tAttack.hitAlly)target = unit;
					//Process attack
					if (target)
					{
						const Vector3 tForce = tAttack.hitForce == Vector3::ZERO ? Vector3::ZERO : tAttack.hitForce.getRotationTo(target->getPosition() - unit->getPosition())*tAttack.hitForce;
						if (target->getType() == OBJECT_CRITTER)
						{
							MagixCritter *tC = static_cast<MagixCritter*>(target);
							mCritterManager->hitCritter(tC, tHP, tForce, 0, tIsInRange);
							if (isPlayer&&!mGameStateManager->isCampaign())mCritterManager->pushHitQueue(HitInfo(tC->getID(), tHP, tForce, tC->imTheOwner()));
							if (tHP<0)tC->setLastHitIndex(0);
						}
						else if (target->getType() == OBJECT_UNIT || target == mPlayer)
						{
							MagixUnit *tUnit = static_cast<MagixUnit*>(target);
							hitUnit(tUnit, tHP, tForce, tIsInRange);
							if (isPlayer&&!mGameStateManager->isCampaign())
							{
								if (target == mPlayer)pushHitQueue(HitInfo(mPlayer->getIndex(), tHP, tForce));
								else if (tUnit->isIndexedUnit())pushHitQueue(HitInfo(static_cast<MagixIndexedUnit*>(tUnit)->getIndex(), tHP, tForce));
							}
						}

						//Attack scripts
						if (isPlayer)
						{
							if (tAttack.name == "Tame")
							{
								mPlayer->setAutoAttack(0);
								if (target->getType() == OBJECT_CRITTER)
								{
									MagixCritter *tC = static_cast<MagixCritter*>(target);
									Critter tCritter = mDef->getCritter(tC->getCritterType());
									const bool tSuccess = ((tC->getMaxHP() != 0 ? Math::UnitRandom()<(10.0f / tC->getMaxHP()) : true)
										&& Math::UnitRandom()>(float)tC->getHP()*0.01f
										&& Math::UnitRandom()>tC->getHPRatio()
										&& (!tCritter.isUncustomizable || Math::UnitRandom()<0.001));
									if (tSuccess)
									{
										tameFlag = TameData(true, true, "Tame success!", tC->getID());
										mSoundManager->playSound(SOUND_HEALHIT, tC->getObjectNode());
										mEffectsManager->createParticle(tC->getObjectNode(), "TameSuccess", 1);
									}
									else tameFlag = TameData(true, false, "Tame failed!");
									mEffectsManager->createParticle(tC->getObjectNode(), "TameHit", 0.25);
								}
								else tameFlag = TameData(true, false, "Target cannot be tamed.");
							}
							if (tAttack.soundRoar)mPlayer->allocFlags.needsSoundRoar = true;
						}
					}
				}
			}
			else
			{
				if (isPlayer || mGameStateManager->isCampaign())mCollisionManager->createCollision(unit->getObjectNode(), tAttack.range, tAttack.hitForce, tAttack.offset, (short)Math::RangeRandom(tAttack.hpMin, tAttack.hpMax), unit->getAlliance(), tAttack.hitAlly);
			}
			if (tAttack.moveForce != Vector3::ZERO)unit->addForce((tAttack.autoTarget ? doAutoTarget(unit, isPlayer) : unit->getObjectNode()->getOrientation())*tAttack.moveForce);
			else if (tAttack.autoTarget && (isPlayer || mGameStateManager->isCampaign()))doAutoTarget(unit, isPlayer);
			unit->setSpeedMultiplier(tAttack.speedMultiplier);
			if (isPlayer&&!mGameStateManager->isCampaign())playerHasNewAttack = true;
		}
		//Resume/stop attack after casting skill once
		if (isPlayer && mPlayer->getAutoAttackOnce())
		{
			if (!mPlayer->getAutoAttackTarget()->matchAlliance(mPlayer->getAlliance()))mPlayer->setAutoAttack(mPlayer->getAutoAttackTarget());
			else mPlayer->setAutoAttack(0);
		}
	}

	//Check hit collisions
	vector<Collision*>::type tHitList = mCollisionManager->getCollisionHitList(unit->getUnitID(), unit->getAlliance(), unit->getBodyEnt()->getWorldBoundingBox());
	for (vector<Collision*>::type::iterator it = tHitList.begin(); it != tHitList.end(); it++)
	{
		Collision *coll = *it;
		//Only if i'm hit or i hit someone (or this is offline)
		if (isPlayer || coll->mNode == mPlayer->getObjectNode() || mGameStateManager->isCampaign())
		{
			const Vector3 tForce = coll->getForce(false) == Vector3::ZERO ? Vector3::ZERO : coll->getForce(true, unit->getPosition());
			hitUnit(unit, coll->hp, tForce, tIsInRange);
			if (!mGameStateManager->isCampaign())
			{
				//I'm hit
				if (isPlayer)pushHitQueue(HitInfo(mPlayer->getIndex(), coll->hp, tForce));
				//I hit someone
				else if (unit->isIndexedUnit())pushHitQueue(HitInfo(static_cast<MagixIndexedUnit*>(unit)->getIndex(), coll->hp, tForce));
			}
			if (coll->hp<0)
			{
				if ((mPlayer->setPlayerTargetOnHit || !mPlayerTarget) && coll->mNode == mPlayer->getObjectNode())setPlayerTarget(unit);
			}
		}
	}
}
bool MagixUnitManager::isInRange(MagixUnit *unit)
{
	if (!unit)return false;
	if (mCamera)
	{
		if (mCamera->getCamera()->isAttached())return(unit->getPosition().squaredDistance(mCamera->getCamera()->getDerivedPosition()) < EARSHOT_SQUARED);
		else return(unit->getPosition().squaredDistance(mCamera->getCamera()->getPosition()) < EARSHOT_SQUARED);
	}
	return false;
}
void MagixUnitManager::hitUnit(MagixUnit *unit, const Real &damage, const Vector3 &force, bool inRange)
{
	if (!unit)return;
	//Check for wounded
	if (unit == mPlayer && mPlayer->getIsWounded())return;
	if (unit->isIndexedUnit() && static_cast<MagixIndexedUnit*>(unit)->isWounded)return;
	if (unit->addHP((short)damage))
	{
		if (unit == mPlayer && !mPlayer->getIsWounded() && !mGameStateManager->isCampaign())mPlayer->setWounded(true);
	}
	if (damage<0)
	{
		if (!unit->isInvulnerable())
		{
			if (inRange || isInRange(unit))
			{
				mEffectsManager->createParticle(unit->getObjectNode(), "Hit1", 0.25);
				mSoundManager->playSound(SOUND_HIT, unit->getObjectNode());
			}
			unit->addForce(force);
			if (unit == mPlayer && !mPlayer->getIsWounded() || mGameStateManager->isCampaign())unit->cancelAction(true, 0.5, mDef);
		}
	}
	else if (damage>0)
	{
		if (inRange || isInRange(unit))
		{
			mEffectsManager->createParticle(unit->getObjectNode(), "HealHit1", 0.25);
			mSoundManager->playSound(SOUND_HEALHIT, unit->getObjectNode());
		}
	}
}
void MagixUnitManager::hitUnit(const OwnerToken &iID, const Real &damage, const Vector3 &force)
{
	MagixUnit *unit = 0;
	if (iID == mPlayer->getIndex())unit = mPlayer;
	else unit = getUnitByIndex(iID);
	hitUnit(unit, damage, force);
}
void MagixUnitManager::updateWallCollisions(MagixUnit *unit)
{
	const vector<Wall*>::type tHitList = mCollisionManager->getWallHitList(unit->getPosition());
	for (vector<Wall*>::type::const_iterator it = tHitList.begin(); it != tHitList.end(); it++)
	{
		Wall *tWall = *it;
		if (tWall->isSphere)
		{
			const Real tX = unit->getPosition().x - tWall->center.x;
			const Real tZ = unit->getPosition().z - tWall->center.z;
			if (Math::Abs(tX)>0.1 && Math::Abs(tZ)>0.1)
			{
				Radian tYaw = Degree(0);
				if (tZ != 0)tYaw = Math::ATan(tX / tZ);
				else tYaw = Degree((Real)(tX >= 0 ? 90 : 270));
				if (tX<0 && tZ >= 0)tYaw += Degree(360);
				if (tZ<0)tYaw += Degree(180);

				unit->addPenaltyVelocity(Vector3(Math::Sin(tYaw)*tWall->range.x - tX, 0, Math::Cos(tYaw)*tWall->range.x - tZ));
			}
		}
		else
		{
			const Real tMinX = tWall->center.x - tWall->range.x / 2;
			const Real tMinZ = tWall->center.z - tWall->range.z / 2;
			const Real tMaxX = tWall->center.x + tWall->range.x / 2;
			const Real tMaxZ = tWall->center.z + tWall->range.z / 2;
			if (tWall->isInside)
			{
				const Real tNearestX = (Math::Abs(tMinX - unit->getPosition().x) <= Math::Abs(tMaxX - unit->getPosition().x) ? tMinX - unit->getPosition().x : tMaxX - unit->getPosition().x);
				const Real tNearestZ = (Math::Abs(tMinZ - unit->getPosition().z) <= Math::Abs(tMaxZ - unit->getPosition().z) ? tMinZ - unit->getPosition().z : tMaxZ - unit->getPosition().z);
				if (Math::Abs(tNearestX) <= Math::Abs(tNearestZ))unit->addPenaltyVelocity(Vector3(tNearestX, 0, 0));
				else unit->addPenaltyVelocity(Vector3(0, 0, tNearestZ));
			}
			else
			{
				if (unit->getPosition().x < tMinX)unit->addPenaltyVelocity(Vector3(tMinX - unit->getPosition().x, 0, 0));
				if (unit->getPosition().x > tMaxX)unit->addPenaltyVelocity(Vector3(tMaxX - unit->getPosition().x, 0, 0));
				if (unit->getPosition().z < tMinZ)unit->addPenaltyVelocity(Vector3(0, 0, tMinZ - unit->getPosition().z));
				if (unit->getPosition().z > tMaxZ)unit->addPenaltyVelocity(Vector3(0, 0, tMaxZ - unit->getPosition().z));
			}
		}
	}
}
void MagixUnitManager::boundUnitToMap(MagixUnit *unit)
{
	const Vector2 tSize = mWorld->getWorldSize();
	const Vector2 tBounds = mWorld->getWorldBounds();
	const Real tMinX = tBounds.x;
	const Real tMinZ = tBounds.y;
	const Real tMaxX = tSize.x - tBounds.x;
	const Real tMaxZ = tSize.y - tBounds.y;

	if (unit->getPosition().x < tMinX)unit->addPenaltyVelocity(Vector3(tMinX - unit->getPosition().x, 0, 0));
	if (unit->getPosition().x > tMaxX)unit->addPenaltyVelocity(Vector3(tMaxX - unit->getPosition().x, 0, 0));
	if (unit->getPosition().z < tMinZ)unit->addPenaltyVelocity(Vector3(0, 0, tMinZ - unit->getPosition().z));
	if (unit->getPosition().z > tMaxZ)unit->addPenaltyVelocity(Vector3(0, 0, tMaxZ - unit->getPosition().z));
}
void MagixUnitManager::updatePortalCollisions(MagixUnit *unit)
{
	Portal *tPortal = mCollisionManager->getPortalHit(unit->getBodyEnt()->getWorldBoundingBox());
	if (tPortal)
	{
		mapChange.changeMap = true;
		mapChange.mapName = tPortal->dest;
		mapChange.changePoint = false;
		tPortal->disable();
	}
}
void MagixUnitManager::updateGateCollisions(MagixUnit *unit)
{
	Gate *tGate = mCollisionManager->getGateHit(unit->getBodyEnt()->getWorldBoundingBox());
	if (tGate)
	{
		mapChange.changeMap = true;
		mapChange.mapName = tGate->dest;
		mapChange.changePoint = true;
		mapChange.hasVectY = tGate->hasVectY;
		mapChange.point = tGate->destVect;
		tGate->disable();
	}
}
void MagixUnitManager::updateCollBoxCollisions(MagixUnit *unit, const FrameEvent &evt)
{
	const Real headHeight = unit->getObjectNode()->getScale().y * 12;
	const vector<CollBox*>::type tHitList = mCollisionManager->getCollBoxHitList(unit->getPosition(), headHeight);
	for (vector<CollBox*>::type::const_iterator it = tHitList.begin(); it != tHitList.end(); it++)
	{
		CollBox *tBox = *it;

		const Real tMaxY = tBox->center.y + tBox->range.y;
		const Real tYCorrection = (unit->getForce().y<0 ? unit->getForce().y*(1 + evt.timeSinceLastFrame) : 0);
		//Stand on box
		if (unit->getPosition().y + GROUND_THRESHOLD - tYCorrection >= tMaxY)
		{
			unit->setGroundHeight(tMaxY);
			unit->allocFlags.hasOwnGroundHeight = true;
		}
		else
		{
			//Head below box
			const Real unitHeight = unit->getPosition().y + headHeight;
			if (unitHeight - GROUND_THRESHOLD <= tBox->center.y)
			{
				unit->addPenaltyVelocity(Vector3(0, tBox->center.y - unitHeight, 0));
				if (unit->getForce().y>0)unit->addForce(Vector3(0, -unit->getForce().y, 0));
			}
			//Blocked by box
			else
			{
				const Real tMinX = tBox->center.x - tBox->range.x / 2;
				const Real tMinZ = tBox->center.z - tBox->range.z / 2;
				const Real tMaxX = tBox->center.x + tBox->range.x / 2;
				const Real tMaxZ = tBox->center.z + tBox->range.z / 2;
				const Real tNearestX = (Math::Abs(tMinX - unit->getPosition().x) <= Math::Abs(tMaxX - unit->getPosition().x) ? tMinX - unit->getPosition().x : tMaxX - unit->getPosition().x);
				const Real tNearestZ = (Math::Abs(tMinZ - unit->getPosition().z) <= Math::Abs(tMaxZ - unit->getPosition().z) ? tMinZ - unit->getPosition().z : tMaxZ - unit->getPosition().z);
				if (Math::Abs(tNearestX) <= Math::Abs(tNearestZ))unit->addPenaltyVelocity(Vector3(tNearestX, 0, 0));
				else unit->addPenaltyVelocity(Vector3(0, 0, tNearestZ));
			}
		}
	}
	const vector<CollSphere*>::type tHitList2 = mCollisionManager->getCollSphereHitList(unit->getPosition());
	for (vector<CollSphere*>::type::const_iterator it = tHitList2.begin(); it != tHitList2.end(); it++)
	{
		CollSphere *tSphere = *it;

		if (tSphere->range == 0)continue;
		const Real tY = tSphere->center.y;
		//Stand on sphere
		if (unit->getPosition().y >= tY)
		{
			const Real tX = unit->getPosition().x - tSphere->center.x;
			const Real tZ = unit->getPosition().z - tSphere->center.z;
			const Real tDist = Vector2(tX, tZ).length();
			if (tDist == 0)
			{
				const Real tHeight = tY + tSphere->range - GROUND_THRESHOLD;
				if (tHeight>unit->getGroundHeight())
				{
					unit->setGroundHeight(tHeight);
					unit->allocFlags.hasOwnGroundHeight = true;
				}
			}
			else
			{
				const Radian tAngle = Math::ACos(tDist / tSphere->range);
				const Real tHeight = tY + Math::Sin(tAngle) * tSphere->range - GROUND_THRESHOLD;
				if (tHeight>unit->getGroundHeight())
				{
					unit->setGroundHeight(tHeight);
					unit->allocFlags.hasOwnGroundHeight = true;
				}
			}
		}
		//Blocked by sphere
		else
		{
			const Real tX = unit->getPosition().x - tSphere->center.x;
			const Real tZ = unit->getPosition().z - tSphere->center.z;
			if (Math::Abs(tX)>0.1 && Math::Abs(tZ)>0.1)
			{
				Radian tYaw = Degree(0);
				if (tZ != 0)tYaw = Math::ATan(tX / tZ);
				else tYaw = Degree((Real)(tX >= 0 ? 90 : 270));
				if (tX<0 && tZ >= 0)tYaw += Degree(360);
				if (tZ<0)tYaw += Degree(180);

				unit->addPenaltyVelocity(Vector3(Math::Sin(tYaw)*tSphere->range - tX, 0, Math::Cos(tYaw)*tSphere->range - tZ));
			}
			//Head under sphere
			if (unit->getForce().y>0)unit->addForce(Vector3(0, -unit->getForce().y, 0));
		}
	}
}
void MagixUnitManager::updateWaterCollisions(MagixUnit *unit)
{
	bool doWaterRipple = false, doSwimming = false, doBubbles = false;
	SceneNode *tNode = unit->getObjectNode();
	WaterBox *tWaterBox = mCollisionManager->getWaterBoxHit(unit->getPosition());

	if (tWaterBox)
	{
		const Real tWaterHeight = tWaterBox->center.y;

		//Solid plane
		if (tWaterBox->isSolid)
		{
			if (unit->getGroundHeight()<tWaterHeight)
				unit->setGroundHeight(tWaterHeight);
		}
		//Permeable plane
		else
		{
			//Head above water, do ripples
			if (unit->getPosition().y + tNode->getScale().x * 12 > tWaterHeight)
			{
				doWaterRipple = true;
				if (!mEffectsManager->hasWaterRippleOwner(tNode))
				{
					mEffectsManager->createWaterRipple(tNode, tWaterHeight, tNode->getScale().x * 10);
				}
				if (!unit->allocFlags.soundWaterWade)
				{
					mSoundManager->playLoopedSound(SOUND_WATERWADE, tNode);
					unit->allocFlags.soundWaterWade = true;
				}
			}
			//Head below water, do bubbles
			else if (unit->getPosition().y + tNode->getScale().x * 15 < tWaterHeight)
			{
				Entity *tHeadEnt = unit->getHeadEnt();
				doBubbles = true;
				if (!mEffectsManager->hasBreatherOwner(tHeadEnt, "Bubbles"))
				{
					const Real tScale = tNode->getScale().x;
					mEffectsManager->createBreather(tHeadEnt, "Bubbles", Vector3(0, (Real)(1.2*tScale), (Real)(-0.4*tScale)), (Real)(tScale*0.2));
				}
			}
			//Splash!
			if (!unit->getIsInWater() && unit->getForce().y<-2 && !unit->getIsSwimming() && !unit->isAntiGravity())
			{
				mEffectsManager->createParticle(unit->getObjectNode(), "WaterSplash", 0.25);
				mSoundManager->playSound(SOUND_SPLASH, unit->getObjectNode());
			}
			//Body below water, do swimming
			if (unit->getPosition().y + tNode->getScale().x * 9 < tWaterHeight)
			{
				unit->setIsSwimming(true);
				unit->setAntiGravity(true);
				doSwimming = true;
			}
			unit->setIsInWater(true);
		}
	}

	if (!doSwimming && unit->getIsSwimming())
	{
		unit->setIsSwimming(false);
		unit->setAntiGravity(false);
		unit->setIsInWater(false);
	}
	if (!doWaterRipple)
	{
		mEffectsManager->setWaterRippleEmissionByOwner(tNode, 0);
		if (unit->allocFlags.soundWaterWade)
		{
			mSoundManager->stopLoopedSoundByNode(tNode, SOUND_WATERWADE);
			unit->allocFlags.soundWaterWade = false;
		}
		unit->setIsInWater(false);
	}
	else
	{
		const Vector3 tForce = unit->getForce();
		mEffectsManager->setWaterRippleEmissionByOwner(tNode, (tForce.x == 0 && tForce.z == 0 && tForce.y >= 0 ? 1 : unit->getSpeedMultiplier() * 10 + Math::RangeRandom(0, 2)));
		mEffectsManager->setWaterRippleHeightByOwner(tNode, tWaterBox->center.y);
	}
	if (!doBubbles)
	{
		mEffectsManager->setBreatherEmissionByOwner(unit->getHeadEnt(), 0, "Bubbles");
	}
	else
	{
		mEffectsManager->setBreatherEmissionByOwner(unit->getHeadEnt(), Math::RangeRandom(0.1, 4), "Bubbles");
	}
}
void MagixUnitManager::updateMapEffects(MagixUnit *unit)
{
	if (mWorld->getMapEffect() != "")
	{
		Entity *tHeadEnt = unit->getHeadEnt();
		if (!unit->allocFlags.mapEffect)
		{
			const Real tScale = unit->getObjectNode()->getScale().x;
			mEffectsManager->createBreather(tHeadEnt, mWorld->getMapEffect(), Vector3(0, (Real)(1.2*tScale), (Real)(-0.4*tScale)), (Real)(tScale * 1));
			unit->allocFlags.mapEffect = true;
		}
	}
}
void MagixUnitManager::updateSounds(MagixUnit *unit)
{
	if ((unit->getForce().x != 0 || unit->getForce().z != 0) && !unit->getIsCrouching() && !unit->isAntiGravity() && !unit->getIsInWater() && unit->distanceFromGround()<GROUND_THRESHOLD)
	{
		if (unit->getIsWalking())
		{
			if (unit->allocFlags.soundRunGrass)
			{
				mSoundManager->stopLoopedSoundByNode(unit->getObjectNode(), SOUND_RUN_GRASS);
				unit->allocFlags.soundRunGrass = false;
			}
			if (!unit->allocFlags.soundWalkGrass)
			{
				mSoundManager->playLoopedSound(SOUND_WALK_GRASS, unit->getObjectNode(), 1, 0.4);
				unit->allocFlags.soundWalkGrass = true;
			}
		}
		else
		{
			if (unit->allocFlags.soundWalkGrass)
			{
				mSoundManager->stopLoopedSoundByNode(unit->getObjectNode(), SOUND_WALK_GRASS);
				unit->allocFlags.soundWalkGrass = false;
			}
			if (!unit->allocFlags.soundRunGrass)
			{
				mSoundManager->playLoopedSound(SOUND_RUN_GRASS, unit->getObjectNode(), 1, 0.4);
				unit->allocFlags.soundRunGrass = true;
			}
		}
	}
	else
	{
		if (unit->allocFlags.soundWalkGrass)
		{
			mSoundManager->stopLoopedSoundByNode(unit->getObjectNode(), SOUND_WALK_GRASS);
			unit->allocFlags.soundWalkGrass = false;
		}
		if (unit->allocFlags.soundRunGrass)
		{
			mSoundManager->stopLoopedSoundByNode(unit->getObjectNode(), SOUND_RUN_GRASS);
			unit->allocFlags.soundRunGrass = false;
		}
	}
	if (unit->allocFlags.needsSoundRoar && unit->isRoaring())
	{
		mSoundManager->playSound((Math::RangeRandom(0, 1)<0.5 ? SOUND_ROAR1 : SOUND_ROAR2), unit->getObjectNode());
		unit->allocFlags.needsSoundRoar = false;
	}
}
bool MagixUnitManager::isMapChanged()
{
	return (mapChange.changeMap);
}
const MapChangeData MagixUnitManager::getMapChange()
{
	return mapChange;
}
void MagixUnitManager::setMapChange(bool changeMap, const String &mapName, bool changePoint, const Vector3 &point, bool hasVectY)
{
	mapChange.changeMap = changeMap;
	mapChange.mapName = (mapName == "" ? mWorld->getWorldName() : mapName);
	mapChange.changePoint = changePoint;
	mapChange.point = point;
	mapChange.hasVectY = hasVectY;
}
const Quaternion MagixUnitManager::doAutoTarget(MagixUnit *unit, bool targetClosest)
{
	//Just yaw to front in first person mode
	if (unit->getControlMode() == CONTROL_FIRSTPERSON)
	{
		unit->setYaw(unit->getLookDirection().getYaw() + Degree(180));
		return unit->getFaceDirection();
	}

	//target autoAttackTarget, if any
	if (unit->getType() == OBJECT_PLAYER && static_cast<MagixPlayer*>(unit)->getAutoAttackTarget())
	{
		unit->setTarget(static_cast<MagixPlayer*>(unit)->getAutoAttackTarget()->getPosition(), TARGET_LOOKAT);
		return (unit->getFaceDirection());
	}

	//target autoTrackObject, if any
	if (unit->getAutoTrackObject())
	{
		unit->setTarget(unit->getAutoTrackObject()->getPosition(), TARGET_LOOKAT);
		return (unit->getFaceDirection());
	}

	//target closest unit
	if (targetClosest)
	{
		//Units
		MagixIndexedUnit *tLast = mLastUnit;
		while (tLast)
		{
			if (!unit->matchAlliance(tLast->getAlliance()) && !tLast->isWounded)
				if (unit->getPosition().squaredDistance(tLast->getPosition()) <= AUTOTARGET_RANGE_SQUARED)
				{
					unit->setTarget(tLast->getPosition(), TARGET_LOOKAT);
					return (unit->getFaceDirection());
				}
			tLast = tLast->getPrevious();
		}
		//Critters
		const vector<MagixCritter*>::type tCritter = mCritterManager->getCritterList();
		for (int i = 0; i<(int)tCritter.size(); i++)
		{
			if (!tCritter[i]->getIsDead())
				if (!unit->matchAlliance(tCritter[i]->getAlliance()))
					if (unit->getPosition().squaredDistance(tCritter[i]->getPosition()) <= AUTOTARGET_RANGE_SQUARED)
					{
						unit->setTarget(tCritter[i]->getPosition(), TARGET_LOOKAT);
						return (unit->getFaceDirection());
					}
		}
	}

	//none
	return unit->getObjectNode()->getOrientation();
}
bool MagixUnitManager::doLockOn()
{
	const bool tTargetAlly = mPlayer->isSkillMode() ? mDef->isSkillTargetsAllies(mPlayer->getCurrentSkill()->name) : false;
	//Units
	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		if (!mPlayer->getIsLockedOn() || (mPlayer->getIsLockedOn() && mPlayer->getAutoTrackObject() != tLast))
			if (!(mPlayer->matchAlliance(tLast->getAlliance()) ^ tTargetAlly) && !tLast->isWounded)
				if (mPlayer->getPosition().squaredDistance(tLast->getPosition()) <= 40000)
				{
					mPlayer->lockOn(tLast);
					setPlayerTarget(tLast);
					return true;
				}
		tLast = tLast->getPrevious();
	}
	//Critters
	const vector<MagixCritter*>::type tCritter = mCritterManager->getCritterList();
	for (int i = 0; i<(int)tCritter.size(); i++)
	{
		if (!tCritter[i]->getIsDead())
			if (!mPlayer->getIsLockedOn() || (mPlayer->getIsLockedOn() && mPlayer->getAutoTrackObject() != tCritter[i]))
				if (!(mPlayer->matchAlliance(tCritter[i]->getAlliance()) ^ tTargetAlly))
					if (mPlayer->getPosition().squaredDistance(tCritter[i]->getPosition()) <= 40000)
					{
						mPlayer->lockOn(tCritter[i]);
						setPlayerTarget(tCritter[i]);
						return true;
					}
	}
	if (!mPlayer->getIsLockedOn())return true;
	else
	{
		mPlayer->lockOn(0);
		setPlayerTarget(0);
	}
	return false;
}
void MagixUnitManager::clampUnitToTerrain(MagixAnimated *unit, bool clampToGround)
{
	Real tHeight = 0;
	static Ray updateRay;
	updateRay.setOrigin(unit->getPosition() + Vector3(0, 5000, 0));
	updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
	mRayQuery->setRay(updateRay);
	RaySceneQueryResult& qryResult = mRayQuery->execute();
	RaySceneQueryResult::iterator i = qryResult.begin();
	if (i != qryResult.end() && i->worldFragment)
	{
		tHeight = i->worldFragment->singleIntersection.y;
	}
	unit->setGroundHeight(tHeight);
	if (clampToGround)unit->setPosition(unit->getPosition().x, tHeight, unit->getPosition().z);
}
void MagixUnitManager::clampUnitToCeiling(MagixUnit *unit)
{
	const Real unitHeight = unit->getPosition().y + unit->getObjectNode()->getScale().y * 12;
	if (unitHeight > mWorld->getCeilingHeight())
	{
		unit->addPenaltyVelocity(Vector3(0, mWorld->getCeilingHeight() - unitHeight, 0));
		unit->addForce(Vector3(0, -unit->getForce().y, 0));
	}
}
const Real MagixUnitManager::getGroundHeight(const Real &x, const Real &z, bool checkWaterCollision, bool checkCollBoxCollision)
{
	Real tHeight = 0;
	static Ray updateRay;
	updateRay.setOrigin(Vector3(x, 5000, z));
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
		const Vector3 tVect = Vector3(x, tHeight, z);
		WaterBox *tWaterBox = mCollisionManager->getWaterBoxHit(tVect);
		if (tWaterBox && tWaterBox->isSolid && tHeight<tWaterBox->center.y)tHeight = tWaterBox->center.y;
	}
	//Check collbox collision
	if (checkCollBoxCollision)
	{
		const Vector3 tVect = Vector3(x, tHeight, z);
		const vector<CollBox*>::type tHitList = mCollisionManager->getCollBoxHitList(tVect, 12);
		for (vector<CollBox*>::type::const_iterator it = tHitList.begin(); it != tHitList.end(); it++)
		{
			CollBox *tBox = *it;
			const Real tMaxY = tBox->center.y + tBox->range.y;
			if (tHeight<tMaxY)tHeight = tMaxY;
		}
		const Vector3 tVect2 = Vector3(x, tHeight, z);
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

	return tHeight;
}
/*void updateGroundHeight(MagixAnimated *unit, const FrameEvent &evt)
{
Real tHeight = 0;
static Ray updateRay;
updateRay.setOrigin(unit->getPosition() + Vector3(0,5000,0));
updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
mRayQuery->setRay(updateRay);
RaySceneQueryResult& qryResult = mRayQuery->execute();
RaySceneQueryResult::iterator i = qryResult.begin();
if(i != qryResult.end() && i->worldFragment)
{
tHeight = i->worldFragment->singleIntersection.y;
}
Real tLength = Vector2(unit->getLastPosition().x-unit->getPosition().x,unit->getLastPosition().z-unit->getPosition().z).length();
if(tLength!=0)
if((tHeight - unit->getGroundHeight())/tLength>=1)
{
unit->setPosition(unit->getLastPosition());
unit->setForce(Vector3(0,unit->getForce().y,0));
}
unit->setGroundHeight(tHeight);
}*/
MagixPlayer* MagixUnitManager::getPlayer()
{
	return mPlayer;
}
MagixIndexedUnit* MagixUnitManager::createUnit(const unsigned short iIndex, const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh)
{
	if (!mLastUnit)
	{
		mLastUnit = new MagixIndexedUnit(iIndex, 0);
		mLastUnit->createUnit(2, mSceneMgr, bodyMesh, headMesh, maneMesh, tailMesh, wingMesh, matName, tuftMesh);
	}
	else
	{
		MagixIndexedUnit *tLast = mLastUnit;
		unsigned short tLastID = mLastUnit->getUnitID();
		mLastUnit = new MagixIndexedUnit(iIndex, tLast);
		mLastUnit->createUnit(tLastID + 1, mSceneMgr, bodyMesh, headMesh, maneMesh, tailMesh, wingMesh, matName, tuftMesh);
	}
	//clampUnitToTerrain(mLastUnit);
	return mLastUnit;
}
MagixIndexedUnit* MagixUnitManager::getUnitByIndex(const unsigned short &iIndex)
{
	MagixIndexedUnit *tLast = mLastUnit;
	MagixIndexedUnit *tTarget = 0;

	while (tLast)
	{
		if (tLast->getIndex() == iIndex)
		{
			tTarget = tLast;
			break;
		}
		else tLast = tLast->getPrevious();
	}

	return tTarget;
}
void MagixUnitManager::deleteUnit(MagixIndexedUnit *target)
{
	if (!target)return;
	if (target == mLastUnit)
	{
		mLastUnit = mLastUnit->getPrevious();
	}
	else
	{
		MagixIndexedUnit *tLast = mLastUnit;
		while (tLast->getPrevious())
		{
			if (tLast->getPrevious() == target)
			{
				MagixIndexedUnit *tPrev = tLast->getPrevious()->getPrevious();
				tLast->setPrevious(tPrev);
				break;
			}
			else tLast = tLast->getPrevious();
		}
	}
	if (target->hasMarker)OverlayManager::getSingleton().destroyOverlayElement("UnitMarker" + StringConverter::toString(target->getUnitID()));
	destroyUnit(target);
	delete target;
}
void MagixUnitManager::deleteAllUnits()
{
	while (mLastUnit)
	{
		MagixIndexedUnit *tLast = mLastUnit;
		mLastUnit = mLastUnit->getPrevious();
		if (tLast->hasMarker)OverlayManager::getSingleton().destroyOverlayElement("UnitMarker" + StringConverter::toString(tLast->getUnitID()));
		destroyUnit(tLast);
		delete tLast;
	}
}
void MagixUnitManager::createNameTag(MagixUnit *target, const String &name)
{
	if (target)target->createNameTag(name, mNameTagAttr);
	if (!shouldNameTagsBeVisible)target->showNameTag(false);
}
void MagixUnitManager::createUserTag(MagixUnit *target, const String &name)
{
	if (target)target->createUserTag(name, mUserTagAttr);
	if (!shouldNameTagsBeVisible)target->showUserTag(false);
}
void MagixUnitManager::createChatBubble(MagixUnit *target, const String &caption)
{
	if (target)target->createChatBubble(caption, mChatBubbleAttr, 0, (Real)(target == mPlayer ? 1 : 0));
}
void MagixUnitManager::updateNameTag(/*RectLayoutManager &m, */MovableTextOverlay *p)
{
	if (!p)return;
	if (p->isOnScreen())
	{
		/*RectLayoutManager::Rect r(	p->getPixelsLeft(),
		p->getPixelsTop(),
		p->getPixelsRight(),
		p->getPixelsBottom());

		RectLayoutManager::RectList::iterator it = m.addData(r);
		if (it != m.getListEnd())
		{
		p->setPixelsTop((*it).getTop());
		p->enable(true);
		}
		else
		p->enable(false);*/
		p->enable(true);
	}
	else
		p->enable(false);
}
void MagixUnitManager::updateChatBubble(/*RectLayoutManager &m, */MovableTextOverlay *p)
{
	if (!p)return;
	if (p->isOnScreen())
	{
		/*RectLayoutManager::Rect r(	p->getPixelsLeft(),
		p->getPixelsTop(),
		p->getPixelsRight(),
		p->getPixelsBottom());

		RectLayoutManager::RectList::iterator it = m.addData(r);
		if (it != m.getListEnd())
		{
		p->setPixelsTop((*it).getTop());
		p->enable(true);
		}
		else
		p->enable(false);*/
		p->enable(true);
	}
	else
		p->enable(false);
}
MagixUnit* MagixUnitManager::getByObjectNode(SceneNode *objectNode)
{
	if (mPlayer->getObjectNode() == objectNode)return mPlayer;

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		if (tLast->getObjectNode() == objectNode)
		{
			return tLast;
		}
		else tLast = tLast->getPrevious();
	}

	return 0;
}
MagixUnit* MagixUnitManager::getByName(const String &name)
{
	if (mPlayer->getName() == name)return mPlayer;

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		if (tLast->getName() == name)
		{
			return tLast;
		}
		else tLast = tLast->getPrevious();
	}

	return 0;
}
MagixUnit* MagixUnitManager::getByUser(String name)
{
	StringUtil::toLowerCase(name);
	String tName = mPlayer->getUser();
	StringUtil::toLowerCase(tName);
	if (tName == name)return mPlayer;

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		tName = tLast->getUser();
		StringUtil::toLowerCase(tName);
		if (tName == name)
		{
			return tLast;
		}
		else tLast = tLast->getPrevious();
	}

	return 0;
}
bool MagixUnitManager::popPlayerTargetChanged()
{
	bool tFlag = playerTargetChanged;
	playerTargetChanged = false;
	return tFlag;
}
MagixLiving* MagixUnitManager::getPlayerTarget()
{
	return mPlayerTarget;
}
void MagixUnitManager::setPlayerTarget(MagixLiving *unit)
{
	playerTargetChanged = (unit != mPlayerTarget);
	mPlayerTarget = unit;
}
void MagixUnitManager::showNameTags(bool flag)
{
	if (!shouldNameTagsBeVisible && flag)return;

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		tLast->showNameTag(flag);
		tLast->showUserTag(flag);
		tLast = tLast->getPrevious();
	}
	mPlayer->showNameTag(flag);
	mPlayer->showUserTag(flag);
}
void MagixUnitManager::setShouldNameTagsBeVisible(bool flag)
{
	shouldNameTagsBeVisible = flag;
	if (!flag)showNameTags(false);
}
bool MagixUnitManager::getShouldNameTagsBeVisible()
{
	return shouldNameTagsBeVisible;
}
void MagixUnitManager::destroyUnit(MagixUnit *unit)
{
	if (unit->getObjectNode())
	{
		MagixIndexedUnit *tLast = mLastUnit;
		while (tLast)
		{
			if (tLast != unit)
				if (tLast->getAutoTrackObject() == unit)tLast->setAutoTrackObject(0);
			tLast = tLast->getPrevious();
		}
		if (mPlayer)
		{
			if (mPlayer->getAutoTrackObject() == unit)mPlayer->lockOn(0);
			if (mPlayer->getAutoAttackTarget() == unit)mPlayer->setAutoAttack(0);
		}
		if (mPlayerTarget == unit)setPlayerTarget(0);
		mCritterManager->removeChaseTarget(unit);
		if (unit->getBodyEnt())mEffectsManager->destroyRibbonTrailByEntity(unit->getBodyEnt());
		mEffectsManager->destroyParticleByObjectNode(unit->getObjectNode());
		mEffectsManager->destroyWaterRippleByOwner(unit->getObjectNode());
		mEffectsManager->destroyBreatherByOwner(unit->getHeadEnt());
		mEffectsManager->destroyItemEffectByOwnerNode(unit->getObjectNode());
		mCollisionManager->destroyCollisionByOwnerNode(unit->getObjectNode());
		mSoundManager->clearChannelsConnectedToSceneNode(unit->getObjectNode());
		unit->allocFlags.clear();
		unit->destroyUnit(mSceneMgr);
	}
}
void MagixUnitManager::createPlayerUnit(const String &bodyMesh, const String &headMesh, const String &maneMesh, const String &tailMesh, const String &wingMesh, const String &matName, const String &tuftMesh)
{
	mPlayer->createUnit(1, mSceneMgr, bodyMesh, headMesh, maneMesh, tailMesh, wingMesh, matName, tuftMesh);
	mPlayer->reset();
	mPlayer->setAlliance(ALLIANCE_FRIEND);
}
const vector<MagixIndexedUnit*>::type MagixUnitManager::getUnitList()
{
	vector<MagixIndexedUnit*>::type tUnitList;
	tUnitList.clear();

	MagixIndexedUnit *tLast = mLastUnit;
	while (tLast)
	{
		tUnitList.push_back(tLast);
		tLast = tLast->getPrevious();
	}

	return tUnitList;
}
ParticleSystem* MagixUnitManager::createParticleOnUnit(MagixUnit *unit, const String &particleName, const Real &duration, bool unresettable)
{
	if (!unit->getObjectNode())return 0;
	return mEffectsManager->createParticle(unit->getObjectNode(), particleName, duration, unresettable);
}
ParticleSystem* MagixUnitManager::createParticleOnUnitBone(MagixUnit *unit, const String &tBone, const String &particleName, const Real &duration, bool unresettable)
{
	if (!unit->getObjectNode())return 0;
	ParticleSystem *pSys = mEffectsManager->createParticle(unit->getObjectNode(), particleName, duration, unresettable, true);
	if (unit->getHeadEnt() && StringUtil::startsWith(tBone, "Head"))
		unit->getHeadEnt()->attachObjectToBone(tBone, pSys, Quaternion::IDENTITY);
	else if (unit->getTailEnt() && StringUtil::startsWith(tBone, "Tail"))
		unit->getTailEnt()->attachObjectToBone(tBone, pSys, Quaternion::IDENTITY);
	else if (unit->getBodyEnt())
		unit->getBodyEnt()->attachObjectToBone(tBone, pSys, Quaternion::IDENTITY);
	return pSys;
}
void MagixUnitManager::createAdminAura(MagixUnit *unit, const ColourValue &colour)
{
	ParticleSystem *aura = createParticleOnUnit(unit, "Aureola", -1, true);
	if (aura)aura->getEmitter(0)->setColour(ColourValue(colour.r, colour.g, colour.b, 0));
}
void MagixUnitManager::destroyParticleOnUnit(MagixUnit *unit, const String &particleName)
{
	mEffectsManager->destroyParticleByObjectNode(unit->getObjectNode(), particleName);
}
bool MagixUnitManager::equipItem(MagixUnit *unit, const String &meshName, const short &slot)
{
	if (slot<0 || slot >= MAX_EQUIP)return false;
	if (unit->getEquip(slot) != "")return false;
	if (!mDef->itemMeshExists(meshName))return false;

	//Offset
	Vector3 offset = Vector3::ZERO;
	if (mDef->getItemHasOffset(meshName))
	{
		if (mDef->getItemBone(meshName) == "Head2")
		{
			char tHeadID = unit->getHeadEnt()->getMesh()->getName().at(4);
			if (tHeadID != '1' && tHeadID != '5')offset = Vector3(0, 0.31, 0.19);
		}
		else if (mDef->getItemBone(meshName) == "Eye.L" || mDef->getItemBone(meshName) == "Eye.R")
		{
			char tHeadID = unit->getHeadEnt()->getMesh()->getName().at(4);
			if (tHeadID != '1' && tHeadID != '5')offset = Vector3(-0.01, 0.0675, 0.35);
		}
	}

	//Alternate animation
	bool altAnim = false;
	if (mDef->getItemHasAltAnim(meshName))
	{
		if (mDef->getItemBone(meshName) == "Head2")
		{
			char tHeadID = unit->getHeadEnt()->getMesh()->getName().at(4);
			if (tHeadID != '1' && tHeadID != '5')altAnim = true;
		}
	}

	unit->equip(mSceneMgr, mDef, meshName, offset, slot, altAnim);

	//Particle effects
	if (mDef->getItemParticle(meshName) != "")
	{
		const String tBone = mDef->getItemBone(meshName);
		Entity *tEnt = 0;
		if (unit->getHeadEnt() && StringUtil::startsWith(tBone, "Head"))
			tEnt = unit->getHeadEnt();
		else if (unit->getTailEnt() && StringUtil::startsWith(tBone, "Tail"))
			tEnt = unit->getTailEnt();
		else tEnt = unit->getBodyEnt();
		if (tEnt)
		{
			const std::pair<Vector3, bool> tOffsetAndOnNode = mDef->getItemParticleOffset(meshName);
			if (tOffsetAndOnNode.second)mEffectsManager->createItemEffect(tEnt, mDef->getItemParticle(meshName), "", tOffsetAndOnNode.first, unit->getObjectNode()->getScale().x);
			else mEffectsManager->createItemEffect(tEnt, mDef->getItemParticle(meshName), tBone, tOffsetAndOnNode.first, unit->getObjectNode()->getScale().x);
		}
	}
	return true;
}
const short MagixUnitManager::getNextEmptyItemSlot(MagixUnit *unit)
{
	for (int i = 0; i<MAX_EQUIP; i++)
		if (unit->getEquip(i) == "")
		{
			return i;
		}
	return -1;
}
const short MagixUnitManager::getNextFilledItemSlot(MagixUnit *unit)
{
	for (int i = 0; i<MAX_EQUIP; i++)
		if (unit->getEquip(i) != "")
		{
			return i;
		}
	return -1;
}
const String MagixUnitManager::unequipItem(MagixUnit *unit, const unsigned short &iID)
{
	const String tItem = unit->unequip(mSceneMgr, mDef, iID);
	//Particle effects
	if (tItem != "")
	{
		const String tItemEffect = mDef->getItemParticle(tItem);
		if (tItemEffect != "")mEffectsManager->destroyItemEffectByOwnerNode(unit->getObjectNode(), tItemEffect);
	}

	return tItem;
}
void MagixUnitManager::createItem(const unsigned short &iID, const String &meshName, const Vector2 &position)
{
	createItem(iID, meshName, position.x, position.y);
}
void MagixUnitManager::createItem(const unsigned short &iID, const String &meshName, const Real &tX, const Real &tZ)
{
	if (!mDef->itemMeshExists(meshName))return;
	const Real tY = getGroundHeight(tX, tZ, true, true);
	mItemManager->createItem(iID, meshName, Vector3(tX, tY, tZ));
}
void MagixUnitManager::deleteItem(const unsigned short &iID)
{
	MagixItem *item = mItemManager->getByID(iID);
	if (!item)return;
	deleteItem(item);
}
void MagixUnitManager::deleteItem(MagixItem *item)
{
	if (mPlayer)
		if (mPlayer->getAutoTrackObject() == item)mPlayer->lockOn(0);
	mItemManager->deleteItem(item->getID());
}
MagixCritter* MagixUnitManager::createCritter(const unsigned short &iID, const unsigned char &worldID, const Vector3 &position, const short &owner)
{
	const WorldCritter tWC = mWorld->getCritterSpawnList(worldID);
	if (tWC.type == "")return 0;
	const Critter tC = mDef->getCritter(tWC.type);
	if (tC.type == "")return 0;
	MagixCritter *tCritter = mCritterManager->createCritter(iID, tC, position, worldID, owner);
	tCritter->setRoamArea(tWC.hasRoamArea);
	if (tWC.hasRoamArea)tCritter->setRoamAreaID((unsigned char)tWC.roamAreaID);
	return tCritter;
}
MagixCritter* MagixUnitManager::createCritter(const unsigned short &iID, const String &type, const Vector3 &position, const short &owner)
{
	const Critter tC = mDef->getCritter(type);
	if (tC.type == "")return 0;
	MagixCritter *tCritter = mCritterManager->createCritter(iID, tC, position, owner);
	return tCritter;
}
void MagixUnitManager::deleteCritter(const unsigned short &iID)
{
	MagixCritter *critter = mCritterManager->getByID(iID);
	if (!critter)return;
	if (mPlayer)
	{
		if (mPlayer->getAutoTrackObject() == critter)mPlayer->lockOn(0);
		if (mPlayer->getAutoAttackTarget() == critter)mPlayer->setAutoAttack(0);
		if (mPlayer->getPetFlags()->attackTarget == critter)mPlayer->getPetFlags()->attackTarget = 0;
	}
	if (mPlayerTarget == critter)setPlayerTarget(0);
	mCritterManager->deleteCritter(iID);
}
const vector<const std::pair<String, Vector2>>::type MagixUnitManager::popItemDropQueue()
{
	const vector<const std::pair<String, Vector2>>::type tList = itemDropQueue;
	itemDropQueue.clear();
	return tList;
}
const String MagixUnitManager::popPickupText()
{
	const String tText = pickupText;
	pickupText = "";
	return tText;
}
void MagixUnitManager::killAndRewardCritter(MagixCritter *critter, bool imTheKiller)
{
	if (!critter)return;
	if (critter->getIsDead())return;
	if (imTheKiller)rewardCritter(critter);
	critter->kill();
	if (mPlayer && mPlayer->getAutoAttackTarget() == critter)mPlayer->setAutoAttack(0);
}
void MagixUnitManager::rewardCritter(MagixCritter *critter)
{
	if (!critter)return;
	//Item drops
	const vector<const std::pair<String, Real>>::type tDropList = mDef->getCritterDropList(critter->getCritterType());
	for (int j = 0; j<(int)tDropList.size(); j++)
		if (Math::UnitRandom()<tDropList[j].second)
		{
			const Vector3 tPos = critter->getPosition() + Vector3(Math::RangeRandom(-10, 10), 0, Math::RangeRandom(-10, 10));
			if (mGameStateManager->isCampaign())createItem(mItemManager->getNextEmptyID(0), tDropList[j].first, Vector2(tPos.x, tPos.z));
			else itemDropQueue.push_back(std::pair<String, Vector2>(tDropList[j].first, Vector2(tPos.x, tPos.z)));
		}
	//Skill drops
	const std::pair<String, unsigned char> tSkill = critter->getIsDrawPoint() ?
		mDef->getRandomSkillDrop() :
		mDef->getCritterSkillDrop(critter->getCritterType());
	if (tSkill.first != "")
	{
		unsigned char tStock = tSkill.second>1 ? (unsigned char)Math::RangeRandom((Real)(tSkill.second / 2), (Real)(tSkill.second)) : 1;
		mPlayer->addSkill(tSkill.first, tStock);
		pickupText = tSkill.first + " x" + StringConverter::toString(tStock);
	}
	if (critter->getIsDrawPoint())
	{
		mEffectsManager->stopParticleByObjectNode(critter->getObjectNode());
		mEffectsManager->createParticle(mPlayer->getObjectNode(), "DrawPoint", 1);
		mSoundManager->playSound(SOUND_DRAW, mPlayer->getObjectNode());
	}
}
bool MagixUnitManager::popPlayerHasNewAttack()
{
	const bool tFlag = playerHasNewAttack;
	playerHasNewAttack = false;
	return tFlag;
}
void MagixUnitManager::addPartyMember(const OwnerToken &token, const String &name)
{
	partyMembers.push_back(std::pair<OwnerToken, String>(token, name));
	partyChanged = true;
}
bool MagixUnitManager::removePartyMember(const String &name)
{
	for (vector<const std::pair<OwnerToken, String>>::type::iterator it = partyMembers.begin(); it != partyMembers.end(); it++)
	{
		const std::pair<OwnerToken, String> tMember = *it;
		if (tMember.second == name)
		{
			partyMembers.erase(it);
			partyChanged = true;
			return true;
		}
	}
	return false;
}
void MagixUnitManager::clearPartyMembers()
{
	partyMembers.clear();
	partyChanged = true;
}
bool MagixUnitManager::isPartyFull()
{
	return (partyMembers.size() >= MAX_PARTYMEMBERS);
}
const vector<const std::pair<OwnerToken, String>>::type MagixUnitManager::getPartyMembers()
{
	return partyMembers;
}
bool MagixUnitManager::isPartyMember(MagixUnit *unit)
{
	if (!unit)return false;
	if (!unit->isIndexedUnit())return false;
	const OwnerToken tToken = (OwnerToken)static_cast<MagixIndexedUnit*>(unit)->getIndex();
	for (int i = 0; i<(int)partyMembers.size(); i++)
		if (partyMembers[i].first == tToken)return true;
	return false;
}
bool MagixUnitManager::popPartyChanged()
{
	const bool tFlag = partyChanged;
	partyChanged = false;
	return tFlag;
}
bool MagixUnitManager::hasParty()
{
	return (partyMembers.size()>0);
}
void MagixUnitManager::setPartyInviter(const OwnerToken &token)
{
	partyInviter = token;
}
const OwnerToken MagixUnitManager::getPartyInviter()
{
	return partyInviter;
}
const std::pair<OwnerToken, String> MagixUnitManager::getPartyMember(const OwnerToken &token)
{
	for (int i = 0; i<(int)partyMembers.size(); i++)
		if (partyMembers[i].first == token)return partyMembers[i];
	return std::pair<OwnerToken, String>(0, "");
}
const vector<const HitInfo>::type MagixUnitManager::popHitQueue()
{
	const vector<const HitInfo>::type tList = hitQueue;
	hitQueue.clear();
	return tList;
}
void MagixUnitManager::pushHitQueue(const HitInfo &info)
{
	hitQueue.push_back(info);
}
const TameData MagixUnitManager::popTameFlag()
{
	const TameData tFlag = tameFlag;
	tameFlag = TameData();
	return tFlag;
}
