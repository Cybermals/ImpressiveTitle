#ifndef __MagixOwnershipManager_h_
#define __MagixOwnershipManager_h_

class MagixOwnershipManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixCritterManager *mCritterManager;
	MagixUnitManager *mUnitManager;
	MagixNetworkManager *mNetworkManager;
	MagixWorld *mWorld;
	MagixChatManager *mChatManager;
	MagixSkyManager *mSkyManager;
	Real ownershipTimer;
	Real spawnTimer;
	Real stationaryCritterTimer;
public:
	MagixOwnershipManager()
	{
		mDef = 0;
		mGameStateManager = 0;
		mCritterManager = 0;
		mUnitManager = 0;
		mNetworkManager = 0;
		mWorld = 0;
		mChatManager = 0;
		mSkyManager = 0;
		ownershipTimer = 0;
		spawnTimer = 0;
		stationaryCritterTimer = 0;
	}
	~MagixOwnershipManager()
	{
	}
	void initialize(MagixExternalDefinitions *def,MagixGameStateManager *gameStateMgr,MagixCritterManager *critterMgr,MagixUnitManager *unitMgr,MagixNetworkManager *networkMgr,MagixWorld *world,MagixChatManager *chatMgr,MagixSkyManager *skyMgr)
	{
		mDef = def;
		mGameStateManager = gameStateMgr;
		mCritterManager = critterMgr;
		mUnitManager = unitMgr;
		mNetworkManager = networkMgr;
		mWorld = world;
		mChatManager = chatMgr;
		mSkyManager = skyMgr;
	}
	void update(const FrameEvent &evt)
	{
		if(!mGameStateManager->isCampaign())updateCritterOwnership(evt);
		updateCritters(evt);
		updateUnits(evt);
	}
	void updateCritterOwnership(const FrameEvent &evt)
	{
		ownershipTimer -= evt.timeSinceLastFrame;
		if(ownershipTimer>0)return;
		ownershipTimer = 2.5;

		const Vector3 tPlayerPosition = mUnitManager->getPlayer()->getPosition();
		const vector<MagixCritter*>::type tCritterList = mCritterManager->getMyCritters();
		vector<MagixCritter*>::type tAbandonList;
		for(int i=0;i<(int)tCritterList.size();i++)
		{
			MagixCritter *tC = tCritterList[i];
			if(!tC->getIsPet() && tC->getPosition().squaredDistance(tPlayerPosition)>4000000)
			{
				tAbandonList.push_back(tC);
			}
		}
		const vector<MagixIndexedUnit*>::type tUnitList = mUnitManager->getUnitList();
		for(int i=0;i<(int)tAbandonList.size();i++)
		{
			MagixCritter *tC = tAbandonList[i];
			MagixIndexedUnit *tSuiter = 0;
			Real tClosestDist = 4000000;
			//Pass ownership to player closest to critter
			for(int j=0;j<(int)tUnitList.size();j++)
			{
				const Real tUnitDist = tC->getPosition().squaredDistance(tUnitList[j]->getPosition());
				if(tUnitDist<tClosestDist)
				{
					tClosestDist = tUnitDist;
					tSuiter = tUnitList[j];
					break;
				}
			}
			if(tSuiter)mNetworkManager->sendCritterTakeover(tC->getID(),tSuiter->getIndex());
		}
	}
	void updateCritters(const FrameEvent &evt)
	{
		if(spawnTimer>0)spawnTimer -= evt.timeSinceLastFrame;

		//Spawn Critters
		if(spawnTimer<=0 && mCritterManager->getNumCritters()<mWorld->getCritterSpawnLimit())
		{
			const vector<const WorldCritter>::type tList = mWorld->getCritterSpawnList();
			if(tList.size()>0)
			{
				unsigned short tID = Math::RangeRandom(0,(int)tList.size());
				if(tID>=(int)tList.size())tID = (int)tList.size()-1;
				const WorldCritter tCritter = tList[tID];
				if(Math::UnitRandom()<(tCritter.spawnRate))
				{
					spawnTimer = Math::RangeRandom(2,5);
					const Critter tC = mDef->getCritter(tCritter.type);
					Real tX,tY,tZ;
					if(tC.isDrawPoint)
					{
						const Vector2 tWorldSize = mWorld->getWorldSize();
						const Vector2 tWorldBounds = mWorld->getWorldBounds();
						tX = Math::RangeRandom(tWorldBounds.x,tWorldSize.x-tWorldBounds.x);
						tZ = Math::RangeRandom(tWorldBounds.y,tWorldSize.y-tWorldBounds.y);
						tY = mUnitManager->getGroundHeight(tX,tZ,true,true);
					}
					else if(tCritter.hasRoamArea)
					{
						const std::pair<Vector3,Vector3> tRoamArea = mWorld->getCritterRoamArea(tCritter.roamAreaID);
						tX = Math::RangeRandom(tRoamArea.first.x,tRoamArea.first.x + tRoamArea.second.x);
						if(tC.flying)tY = Math::RangeRandom(tRoamArea.first.y,tRoamArea.first.y + tRoamArea.second.y);
						tZ = Math::RangeRandom(tRoamArea.first.z,tRoamArea.first.z + tRoamArea.second.z);
						const Real tGroundHeight = mUnitManager->getGroundHeight(tX,tZ,true,true);
						if(!tC.flying || tGroundHeight>tY)tY = tGroundHeight;
					}
					else
					{
						const Vector3 tPlayerPosition = mUnitManager->getPlayer()->getPosition();
						tX = tPlayerPosition.x + Math::RangeRandom(-2000,2000);
						tZ = tPlayerPosition.z + Math::RangeRandom(-2000,2000);
						tY = tC.flying?Math::RangeRandom(400,800):mUnitManager->getGroundHeight(tX,tZ,true,true);
					}
					if(mGameStateManager->isCampaign())mUnitManager->createCritter(mCritterManager->getNextEmptyID(0),tID,Vector3(tX,tY,tZ));
					else mNetworkManager->sendCritterCreate(tID,Vector3(tX,tY,tZ));
				}
			}
		}

		//Clear dead critters
		const vector<MagixCritter*>::type tDList = mCritterManager->popDeadQueue();
		for(int i=0;i<(int)tDList.size();i++)mUnitManager->deleteCritter(tDList[i]->getID());


		//Update staionary critters AI
		if(stationaryCritterTimer>0)stationaryCritterTimer -= evt.timeSinceLastFrame;
		if(stationaryCritterTimer<=0)
		{
			stationaryCritterTimer = 0.5;
			const vector<MagixCritter*>::type tCritterStationaryList = mCritterManager->popStationaryQueue();
			for(int i=0;i<(int)tCritterStationaryList.size();i++)
			{
				MagixCritter *tC = tCritterStationaryList[i];
				if(tC->getHP()<=0)continue;
				//From player
				const Real tSquaredDist = tC->getPosition().squaredDistance(mUnitManager->getPlayer()->getPosition());
				if(tSquaredDist<250000 && (!mUnitManager->getPlayer()->getIsCrouching() || mUnitManager->getPlayer()->isAttacking() ||  mUnitManager->getPlayer()->isAntiGravity()) && Math::UnitRandom()<0.05)
				{
					if(tC->getDecisionTimer()>1)tC->setDecisionTimer(1);
					continue;
				}
				if(tSquaredDist<90000 && Math::UnitRandom()<0.002){tC->setDecisionTimer(tC->getDecisionTimer()+1); continue;}
				//From other units
				const vector<MagixIndexedUnit*>::type tUnitList = mUnitManager->getUnitList();
				for(int j=0;j<(int)tUnitList.size();j++)
				{
					MagixIndexedUnit *tUnit = tUnitList[j];
					const Real tSquaredDist2 = tC->getPosition().squaredDistance(tUnit->getPosition());
					if(tSquaredDist2<250000 && (!tUnit->getIsCrouching() || tUnit->isAttacking() || tUnit->isAntiGravity()) && Math::UnitRandom()<0.05)
					{
						if(tC->getDecisionTimer()>1)tC->setDecisionTimer(1);
						break;
					}
					if(tSquaredDist2<90000 && Math::UnitRandom()<0.002){tC->setDecisionTimer(tC->getDecisionTimer()+1); break;}
				}
			}
		}

		//Update and send critter decisions
		const vector<MagixCritter*>::type tCritterDecisionList = mCritterManager->popDecisionQueue();
		for(int i=0;i<(int)tCritterDecisionList.size();i++)
		{
			MagixCritter *tC = tCritterDecisionList[i];
			//Pet decisions
			if(tC->getIsPet() && mUnitManager->getPlayer()->getPet()==tC->getCritterType())
			{
				PetFlags *tPetFlags = mUnitManager->getPlayer()->getPetFlags();
				if(!tPetFlags->stay)
				{
					if(tC->getHasAttack() && tPetFlags->attackTarget && tC->getHP()>0)
					{
						const bool tFriendlyAttack = (tPetFlags->attackTarget->matchAlliance(tC->getAlliance()));
						const unsigned char tAttackID = mDef->getCritterRandomSpecificAttack(tC->getCritterType(),!tFriendlyAttack);
						if(tAttackID!=0)
						{
							tC->setTarget(tPetFlags->attackTarget,tAttackID);
							if(!mGameStateManager->isCampaign())
							{
								if(tPetFlags->attackTarget->getType()==OBJECT_CRITTER)
									mNetworkManager->sendCritterAttackDecision(tC->getID(),static_cast<MagixCritter*>(tPetFlags->attackTarget)->getID(),tAttackID,true);
								else if(tPetFlags->attackTarget->getType()==OBJECT_UNIT && static_cast<MagixUnit*>(tPetFlags->attackTarget)->isIndexedUnit())
									mNetworkManager->sendCritterAttackDecision(tC->getID(),static_cast<MagixIndexedUnit*>(tPetFlags->attackTarget)->getIndex(),tAttackID);
								else if(tPetFlags->attackTarget==mUnitManager->getPlayer())
									mNetworkManager->sendCritterAttackDecision(tC->getID(),mUnitManager->getPlayer()->getIndex(),tAttackID);
							}
						}
					}
					else
					{
						const Vector3 tPlayerPos = mUnitManager->getPlayer()->getPosition();
						Real tDist = tC->getLength()*0.8;
						if(tDist<30)tDist = 30;
						const Real tHeight = 12+tC->getHeight();
						if(tC->getPosition().squaredDistance(tPlayerPos)>Math::Sqr(tDist*2))
						{
							tC->setTarget(tPlayerPos + Vector3(Math::RangeRandom(-tDist,tDist),tC->isAntiGravity()?tHeight:0,Math::RangeRandom(-tDist,tDist)));
							if(!mGameStateManager->isCampaign())mNetworkManager->sendCritterDecision(tC->getID(),tC->getTarget());
						}
					}
					tC->setDecisionTimer(1+Math::UnitRandom());
				}
				tPetFlags->attackTarget = 0;
			}
			//Normal critter decisions
			else
			{
				const Critter tInfo = mDef->getCritter(tC->getCritterType());

				const short tLastHitIndex = tC->getLastHitIndex();
				MagixUnit *tTargetUnit = 0;
				if(tLastHitIndex==0)
				{
					if(!mUnitManager->getPlayer()->getIsWounded())tTargetUnit = mUnitManager->getPlayer();
				}
				else if(tLastHitIndex!=-1)
				{
					MagixIndexedUnit *tUnit = mUnitManager->getUnitByIndex(tLastHitIndex);
					if(tUnit && !tUnit->isWounded)tTargetUnit = tUnit;
				}

				//Do attack
				if(tC->getHasAttack() && tTargetUnit && Math::UnitRandom()<0.7)
				{
					bool tHitAlly = false;
					const unsigned char tAttackID = mDef->getCritterRandomAttack(tC->getCritterType(),&tHitAlly);
					//Heal self
					if(tHitAlly)
					{
						tC->setTarget(tC,tAttackID);
						if(!mGameStateManager->isCampaign())mNetworkManager->sendCritterAttackDecision(tC->getID(),tC->getID(),tAttackID,true);
					}
					//Attack enemy
					else
					{
						const OwnerToken tTargetID = tLastHitIndex==0?mUnitManager->getPlayer()->getIndex():tLastHitIndex;
						
						tC->setTarget(tTargetUnit,tAttackID);
						if(tC->getRoamArea())
						{
							const std::pair<Vector3,Vector3> tRoamArea = mWorld->getCritterRoamArea(tC->getRoamAreaID());
							tC->setStopChaseLimit(tRoamArea.first.y + tRoamArea.second.y);
						}
						if(!mGameStateManager->isCampaign())mNetworkManager->sendCritterAttackDecision(tC->getID(),tTargetID,tAttackID);
					}
				}
				//Roam about
				else
				{
					if(tC->getRoamArea())
					{
						const std::pair<Vector3,Vector3> tRoamArea = mWorld->getCritterRoamArea(tC->getRoamAreaID());
						const Real tX = Math::RangeRandom(tRoamArea.first.x,tRoamArea.first.x + tRoamArea.second.x);
						const Real tY = Math::RangeRandom(tRoamArea.first.y,tRoamArea.first.y + tRoamArea.second.y);
						const Real tZ = Math::RangeRandom(tRoamArea.first.z,tRoamArea.first.z + tRoamArea.second.z);
						tC->setTarget(Vector3(tX,tY,tZ));
					}
					else
					{
						tC->setTarget(tC->getPosition() + Vector3(Math::RangeRandom(-300,300),tC->isAntiGravity()?Math::RangeRandom(-50,50):0,Math::RangeRandom(-300,300)));
					}
					if(!mGameStateManager->isCampaign())mNetworkManager->sendCritterDecision(tC->getID(),tC->getTarget());
				}
				tC->setDecisionTimer(Math::RangeRandom(tInfo.decisionMin,tInfo.decisionMin+tInfo.decisionDeviation));
			}
		}

		
		PetFlags *tPetFlags = mUnitManager->getPlayer()->getPetFlags();
		//Pet heals player if possible
		if(mUnitManager->getPlayer()->getPet()!="" && !tPetFlags->attackTarget && tPetFlags->hasHeal && mUnitManager->getPlayer()->getHPRatio()<1 && !mUnitManager->getPlayer()->getIsWounded() && Math::UnitRandom()<0.1)
		{
			tPetFlags->attackTarget = mUnitManager->getPlayer();
		}
		//Player has target
		MagixLiving *tCritter = mCritterManager->popPlayerTarget();
		if(tCritter)
		{
			//Set critter hit by player to be target, if any
			if(mUnitManager->getPlayer()->setPlayerTargetOnHit||!mUnitManager->getPlayerTarget())mUnitManager->setPlayerTarget(tCritter);
			//Set critter hit to be pet target
			if(!tCritter->matchAlliance(mUnitManager->getPlayer()->getAlliance()) && mUnitManager->getPlayer()->getPet()!="" && !tPetFlags->attackTarget)tPetFlags->attackTarget = tCritter;
		}
		if(mUnitManager->getPlayerTarget() && mUnitManager->getPlayerTarget()->getType()==OBJECT_CRITTER)
			if(static_cast<MagixCritter*>(mUnitManager->getPlayerTarget())->getIsDead())mUnitManager->setPlayerTarget(0);
		
		if(tPetFlags->attackTarget && tPetFlags->attackTarget->getType()==OBJECT_CRITTER)
			if(static_cast<MagixCritter*>(tPetFlags->attackTarget)->getIsDead())tPetFlags->attackTarget = 0;


		//Unlock-on dead critters
		if(mUnitManager->getPlayer()->getIsLockedOn() && mUnitManager->getPlayer()->getAutoTrackObject() && mUnitManager->getPlayer()->getAutoTrackObject()->getType()==OBJECT_CRITTER)
			if(static_cast<MagixCritter*>(mUnitManager->getPlayer()->getAutoTrackObject())->getIsDead())mUnitManager->getPlayer()->lockOn(0);

		if(!mGameStateManager->isCampaign())
		{
			//HitInfo
			const vector<const HitInfo>::type tCritterHitList = mCritterManager->popHitQueue();
			for(int i=0;i<(int)tCritterHitList.size();i++)
			{
				mNetworkManager->sendCritterHit(tCritterHitList[i].ID,tCritterHitList[i].hp,tCritterHitList[i].force);
				if(!tCritterHitList[i].isMine)mNetworkManager->sendCritterTakeover(tCritterHitList[i].ID);
			}
			//Itemdrop
			const vector<const std::pair<String,Vector2>>::type tItemList = mUnitManager->popItemDropQueue();
			for(int i=0;i<(int)tItemList.size();i++)
			{
				const Vector2 tPos = tItemList[i].second;
				mNetworkManager->sendItemDrop(tItemList[i].first,tPos.x,tPos.y);
			}
		}

		//Reward for killed critters
		const vector<MagixCritter*>::type tKList = mCritterManager->popKilledQueue();
		for(int i=0;i<(int)tKList.size();i++)
		{
			//Disable killed pets (i.e. wounded)
			if(tKList[i]->getIsPet())
			{
				tKList[i]->setHPRatio(0);
				tKList[i]->setTarget(0);
				mChatManager->message("Your pet is wounded! It will not engage in battle until you cure it.");
			}
			else
			{
				if(mGameStateManager->isCampaign())mUnitManager->killAndRewardCritter(tKList[i]);
				else mNetworkManager->sendCritterKilled(tKList[i]->getID(),tKList[i]->getLastHitIndex());
			}
		}

		//Critter effects
		const String tWeatherEffect = mCritterManager->popWeatherEffectRequest();
		if(tWeatherEffect!="")mSkyManager->setWeatherEffect(tWeatherEffect);
	}
	void updateUnits(const FrameEvent &evt)
	{
		//HitInfo
		if(!mGameStateManager->isCampaign())
		{
			const vector<const HitInfo>::type tUnitHitList = mUnitManager->popHitQueue();
			for(int i=0;i<(int)tUnitHitList.size();i++)
			{
				mNetworkManager->sendPlayerHit(tUnitHitList[i].ID,tUnitHitList[i].hp,tUnitHitList[i].force);
			}
		}
	}
};

#endif