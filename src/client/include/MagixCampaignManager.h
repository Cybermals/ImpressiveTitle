#ifndef __MagixCampaignManager_h_
#define __MagixCampaignManager_h_

struct ScriptedUnit
{
	MagixUnit *mUnit;
	String script;
	Real attackFrequency;
	MagixUnit *chaseUnit;
	Real timer;
	CampaignEventList eventList;
	vector<const String>::type nextEvent;
	vector<vector<const String>::type>::type eventConditionList;
	ScriptedUnit()
	{
		mUnit = 0;
		script = "";
		attackFrequency = 0;
		chaseUnit = 0;
		timer = 0;
		eventList.clear();
		nextEvent.clear();
		eventConditionList.clear();
	}
};

class MagixCampaignManager
{
protected:
	MagixExternalDefinitions *mDef;
	MagixGameStateManager *mGameStateManager;
	MagixGUI *mGui;
	MagixUnitManager *mUnitManager;
	MagixWorld *mWorld;
	MagixSkyManager *mSkyManager;
	MagixCamera *mCamera;
	MagixSoundManager *mSoundManager;
	MagixCollisionManager *mCollisionManager;
	MagixEffectsManager *mEffectsManager;
	unsigned short progress;
	unsigned short progressCheckpoint;
	Real cinematicTimer;
	bool randomSpawn;
	Real randomSpawnTimer;
	vector<MagixUnit*>::type mSpawn;
	vector<ScriptedUnit>::type mScriptedUnits;
	vector<String>::type credits;
	Real creditsTimeout;
	unsigned short creditsCounter;
	Real gameOverTimeout;
	vector<Skill>::type skillsCheckpoint;
	String campaignFilename;
	CampaignEventList eventList;
	vector<const String>::type nextEvent;
	vector<vector<const String>::type>::type eventConditionList;
	bool ignoreGameOver;
public:
	MagixCampaignManager()
	{
		mDef = 0;
		mGameStateManager = 0;
		mGui = 0;
		mUnitManager = 0;
		mWorld = 0;
		mSkyManager = 0;
		mCamera = 0;
		mSoundManager = 0;
		mCollisionManager = 0;
		mEffectsManager = 0;
		progress = 0;
		progressCheckpoint = 0;
		cinematicTimer = 0;
		randomSpawn = false;
		randomSpawnTimer = 0;
		mSpawn.clear();
		mScriptedUnits.clear();
		credits.clear();
		creditsTimeout = 0;
		creditsCounter = 0;
		gameOverTimeout = 0;
		skillsCheckpoint.clear();
		campaignFilename = "";
		eventList.clear();
		nextEvent.clear();
		eventConditionList.clear();
		ignoreGameOver = false;
	}
	~MagixCampaignManager()
	{
		mScriptedUnits.clear();
	}
	void initialize(MagixExternalDefinitions *def, MagixGameStateManager *gameStateMgr,	MagixGUI *gui, MagixUnitManager *unitMgr, MagixWorld *world, MagixSkyManager *skyMgr, MagixCamera *camera, MagixSoundManager *soundMgr, MagixCollisionManager *collisionMgr, MagixEffectsManager *fxMgr)
	{
		mDef = def;
		mGameStateManager = gameStateMgr;
		mGui = gui;
		mUnitManager = unitMgr;
		mWorld = world;
		mSkyManager = skyMgr;
		mCamera = camera;
		mSoundManager = soundMgr;
		mCollisionManager = collisionMgr;
		mEffectsManager = fxMgr;
	}
	void loadCampaign()
	{
		progress = 0;
		progressCheckpoint = 0;
		randomSpawn = false;
		randomSpawnTimer = 0;
		skillsCheckpoint.clear();
		ignoreGameOver = false;
		mGui->getChatManager()->reset(true);

		CampaignEventList tEventList;
		mDef->loadCampaign(mGameStateManager->getCampaignName(),tEventList,campaignFilename);
		if(tEventList.size()<=0)return;
		const CampaignEvent tInit = tEventList[0];
		processEvent(tInit,true);

		eventList.clear();
		eventConditionList.clear();
		mDef->loadCampaignScript(campaignFilename,nextEvent,eventList,progress+1);
		for(int i=0;i<(int)eventList.size();i++)registerEventConditions(eventList[i]);
	}
	void update(const FrameEvent &evt)
	{
		if(cinematicTimer>0)
		{
			cinematicTimer -= evt.timeSinceLastFrame;
			if(cinematicTimer<0)cinematicTimer=0;
		}
		updateSpawns();
		if(randomSpawn && mSpawn.size()==0)
		{
			if(randomSpawnTimer==0)randomSpawnTimer = Math::RangeRandom(5,10);
			randomSpawnTimer -= evt.timeSinceLastFrame;
			if(randomSpawnTimer<=0)
			{
				randomSpawnTimer = 0;
				doRandomSpawn();
			}
		}
		if(!checkGameOver(evt))checkEventCondition();
		updateScriptedUnits(evt);
	}
	void updateScriptedUnits(const FrameEvent &evt)
	{
		for(vector<ScriptedUnit>::type::iterator it=mScriptedUnits.begin();it!=mScriptedUnits.end();it++)
		{
			ScriptedUnit *sUnit = &*it;
			if(sUnit->timer>0)sUnit->timer -= evt.timeSinceLastFrame;
			if(sUnit->timer<0)sUnit->timer = 0;
			checkEventCondition(sUnit);
			if(sUnit->attackFrequency>0 && Math::UnitRandom()<sUnit->attackFrequency)sUnit->mUnit->doAttack(mDef);
			if(sUnit->chaseUnit)
			{
				MagixUnit *unit = sUnit->mUnit;
				MagixUnit *target = sUnit->chaseUnit;
				if(Vector2(unit->getPosition().x-target->getPosition().x,unit->getPosition().z-target->getPosition().z).squaredLength()>2500)
				unit->setTarget(Vector3(target->getPosition().x,unit->getPosition().y,target->getPosition().z),TARGET_RUNTO);
			}
		}
	}
	void continueCampaign()
	{
		mScriptedUnits.clear();
		mCollisionManager->reset();
		mUnitManager->reset(false);
		mUnitManager->getPlayer()->resetTarget();
		mUnitManager->getPlayer()->setHPRatio(1);
		mUnitManager->getPlayer()->setCurrentSkill(0);
		if(mUnitManager->getPlayer()->getIsStance())mUnitManager->getPlayer()->toggleStance(mDef);
		mCamera->reset(false);
		mGameStateManager->setGameState(GAMESTATE_INGAMECAMPAIGN);

		progress = progressCheckpoint;
		if(progress!=0)mUnitManager->getPlayer()->setSkills(skillsCheckpoint);
		cinematicTimer = 0;
		randomSpawn = false;
		randomSpawnTimer = 0;

		nextEvent.clear();
		eventList.clear();
		eventConditionList.clear();
		mDef->loadCampaignScript(campaignFilename,nextEvent,eventList,progress+1);
		for(int i=0;i<(int)eventList.size();i++)registerEventConditions(eventList[i]);
	}
	bool checkGameOver(const FrameEvent &evt)
	{
		if(ignoreGameOver)return false;
		if(gameOverTimeout==0 && mUnitManager->getPlayer()->getHPRatio()<=0)
		{
			if(!mGameStateManager->isCinematic())toggleCinematic(true);
			mUnitManager->getPlayer()->setDeathTimeout(1);
			mGui->fadeOut(1,ColourValue(1,1,1));
			mSoundManager->stopMusic(true);
			gameOverTimeout = 1.5;
			return true;
		}
		if(gameOverTimeout>0)
		{
			gameOverTimeout -= evt.timeSinceLastFrame;
			if(gameOverTimeout<=0)
			{
				gameOverTimeout = 0;
				mUnitManager->getPlayer()->setDeathTimeout(0);
				killSpawns();
				toggleCinematic(true);
				mGui->fadeIn(0.5,ColourValue(1,1,1));
				mGui->showCreditsOverlay(true,false,true);
				mGui->showMouse(true);
				mGameStateManager->setGameState(GAMESTATE_GAMEOVER);
			}
			return true;
		}
		return false;
	}
	void doRandomSpawn()
	{
		unsigned short tSpawns = 0;
		Real tRnd = Math::UnitRandom();
		if(tRnd<0.6)tSpawns = 1;
		else tSpawns = 2;

		createSpawns(tSpawns);
	}
	void createSpawns(unsigned short number)
	{
		for(int i=0;i<number;i++)
		{
			MagixUnit *shadow;
			shadow = mUnitManager->createUnit(i+1,"Body","Head1","Mane2","Tail1","Wingless","CustomMat","Tuftless");
			shadow->setColours(ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(0,0,0),ColourValue(1,1,1),ColourValue(0,0,0),ColourValue(0,0,0));
			shadow->setPosition(mUnitManager->getPlayer()->getPosition()+Vector3(Math::RangeRandom(-100,100),100,Math::RangeRandom(-100,100)));
			shadow->resetTarget();
			shadow->setTarget(mUnitManager->getPlayer()->getPosition(),TARGET_LOOKAT);
			shadow->setAutoTrackObject(mUnitManager->getPlayer());
			shadow->setAlliance(ALLIANCE_ENEMY);
			mUnitManager->createNameTag(shadow,"Shadow");
			mUnitManager->clampUnitToTerrain(shadow,true);
			shadow->setHP(Math::RangeRandom(100,150));
			shadow->pushAttackList("Dash");
			shadow->pushAttackList("Roundhouse");
			mSpawn.push_back(shadow);
		}
	}
	void updateSpawns()
	{
		for(vector<MagixUnit*>::type::iterator it = mSpawn.begin();it != mSpawn.end(); it++)
		{
			MagixUnit *tUnit = *it;
			
			if(tUnit->isDead())
			{
				mUnitManager->deleteUnit(dynamic_cast<MagixIndexedUnit*>(tUnit));
				mSpawn.erase(it);
				break;
			}
			if(!tUnit->isDying() && tUnit->getHPRatio()<=0)
			{
				tUnit->setDeathTimeout(1);
				tUnit->stopAction();
				tUnit->setAutoTrackObject(0);
				mUnitManager->getPlayer()->addSkill("Cure",1);
				mGui->showPickupText("Cure x1");
				if(mUnitManager->getPlayer()->getCurrentSkill() && mUnitManager->getPlayer()->getCurrentSkill()->name=="Cure")mGui->updateSkillText(mUnitManager->getPlayer()->getCurrentSkill());
			}
			else if(tUnit->isDying())
			{
				const Real tRatio = tUnit->getDeathTimeout();
				tUnit->setFullColour(ColourValue(0,0,0,tRatio));
			}
			else
			{
				if(tUnit->getPosition().squaredDistance(mUnitManager->getPlayer()->getPosition())>2500)tUnit->setTarget(mUnitManager->getPlayer()->getPosition(),TARGET_RUNTO);
				if(Math::UnitRandom()>0.97)tUnit->doAttack(mDef);
			}
		}
	}
	void killSpawns()
	{
		for(vector<MagixUnit*>::type::iterator it = mSpawn.begin();it != mSpawn.end(); it++)
		{
			MagixUnit *tUnit = *it;
			tUnit->kill();
		}
		while(mSpawn.size()>0)
		{
			updateSpawns();
		}
	}
	bool unitIsInRegion(MagixUnit *unit, const Vector2 &center, const Real &range)
	{
		if(!unit)return false;
		Sphere tSphere(Vector3(center.x,unit->getPosition().y,center.y),range);
		return tSphere.intersects(unit->getPosition());
	}
	void setCheckpoint()
	{
		progressCheckpoint = progress;
		skillsCheckpoint = mUnitManager->getPlayer()->getSkills();
	}
	void toggleCinematic(bool skipTransition=false)
	{
		mGameStateManager->toggleCinematic();
		mUnitManager->getPlayer()->stopAction();
		mGui->showGUI(!mGameStateManager->isCinematic());
		mGui->showCinematicBars(mGameStateManager->isCinematic(),skipTransition);
	}
	void closeCampaign(unsigned short restartState=GAMESTATE_INITIALIZE, const String &creditsName="")
	{
		mGui->getChatManager()->reset(true);
		mSpawn.clear();
		mScriptedUnits.clear();
		mCamera->reset();
		mCollisionManager->reset();
		mUnitManager->reset(true);
		mSoundManager->reset();
		mWorld->unloadWorld();
		mSkyManager->showSky(false);
		if(mGameStateManager->isCinematic())mGameStateManager->toggleCinematic();
		if(mGameStateManager->isPaused())mGameStateManager->togglePause();
		mGameStateManager->setCampaign("");
		mGameStateManager->setGameState(restartState);
		if(restartState==GAMESTATE_CREDITS)
		{
			if(!mDef->loadCredits(creditsName,credits))mGameStateManager->setGameState(GAMESTATE_INITIALIZE);
		}
		else if(restartState==GAMESTATE_STARTSCREEN)mGui->resetCampaignScreen();
	}
	void updateCredits(const FrameEvent &evt)
	{
		if(creditsCounter==0 && credits.size()>0)
		{
			mGui->showCreditsOverlay(true);
			mSoundManager->playMusic(credits[0].c_str());
			creditsCounter++;
			creditsTimeout = -0.5;
		}
		if(creditsTimeout>0)
		{
			creditsTimeout -= evt.timeSinceLastFrame;
			if(creditsTimeout<=0)
			{
				creditsTimeout = -0.5;
				mGui->fadeOut(0.5);
			}
			return;
		}
		if(creditsTimeout<0)
		{
			creditsTimeout += evt.timeSinceLastFrame;
			if(creditsTimeout>=0)
			{
				if(credits.size()>creditsCounter)
				{
					mGui->showCreditsCaption(credits[creditsCounter]);
					creditsTimeout = 5.5;
					creditsCounter++;
					mGui->fadeIn(0.5);
				}
				else
				{
					creditsTimeout = 0;
					creditsCounter = 0;
					credits.clear();
					mGui->showCreditsOverlay(false);
					mGameStateManager->setGameState(GAMESTATE_INITIALIZE);
				}
			}
			return;
		}
	}
	bool compare(String first, String second)
	{
		StringUtil::toLowerCase(first);
		StringUtil::toLowerCase(second);
		return first==second;
	}
	void processEvent(const CampaignEvent &cEvent, bool isInitialize=false, ScriptedUnit *sUnit=0)
	{
		bool tProgressChanged = false;
		for(int i=0;i<(int)cEvent.size();i++)
		{
			if(compare(cEvent[i].first,"World_Load"))mWorld->loadWorld(cEvent[i].second);
			else if(compare(cEvent[i].first,"World_DestroyAllPortals"))
			{
				mCollisionManager->destroyAllPortals();
				mCollisionManager->destroyAllGates();
				mGui->getMiniMap()->updatePortalMap();
			}
			else if(compare(cEvent[i].first,"World_CreateBoxWall"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()==3)mCollisionManager->createBoxWall(parseVector3(tPart[0]),parseVector3(tPart[1]),StringConverter::parseBool(tPart[2]));
			}
			else if(compare(cEvent[i].first,"World_CreateSphereWall"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()==3)mCollisionManager->createSphereWall(parseVector3(tPart[0]),StringConverter::parseReal(tPart[1]),StringConverter::parseBool(tPart[2]));
			}
			else if(compare(cEvent[i].first,"World_DestroyWall"))mCollisionManager->destroyWall(StringConverter::parseInt(cEvent[i].second));
			else if(compare(cEvent[i].first,"World_CreatePortal"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=2)
				{
					mCollisionManager->createPortal(parseVector3(tPart[0],sUnit),tPart.size()>=3?parseReal(tPart[2]):25,tPart[1]);
					mGui->getMiniMap()->updatePortalMap();
				}
			}
			else if(compare(cEvent[i].first,"World_CreateGate"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=3)
				{
					mCollisionManager->createGate(parseVector3(tPart[0],sUnit),tPart[1],parseVector3(tPart[2],sUnit),tPart.size()>=4?tPart[3]:"GateMatBlack",(tPart.size()>=5?StringConverter::parseBool(tPart[4]):false));
					mGui->getMiniMap()->updatePortalMap();
				}
			}
			else if(compare(cEvent[i].first,"World_ClearCritterList"))
			{
				mWorld->clearCritterSpawnList();
			}
			else if(compare(cEvent[i].first,"Sky_DayTime"))mSkyManager->setDayTime(parseReal(cEvent[i].second));
			else if(compare(cEvent[i].first,"Sky_SetWeatherCycle"))mSkyManager->setWeatherCycle(cEvent[i].second);
			else if(compare(cEvent[i].first,"Sky_FreezeTime"))mSkyManager->freezeDayTime(StringConverter::parseBool(cEvent[i].second));
			else if(compare(cEvent[i].first,"Sky_WeatherTime"))mSkyManager->setWeatherTime(parseReal(cEvent[i].second));
			else if(compare(cEvent[i].first,"Unit_Create"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",8);
				if(tPart.size()==9)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit==mUnitManager->getPlayer())
					{
						mUnitManager->createPlayerUnit(tPart[2],tPart[3],tPart[4],tPart[5],tPart[6],tPart[7],tPart[8]);
						mUnitManager->resetPlayerPosition();
					}
					else
					{
						unit = mUnitManager->createUnit(StringConverter::parseInt(tPart[0]),tPart[2],tPart[3],tPart[4],tPart[5],tPart[6],tPart[7],tPart[8]);
					}
					mUnitManager->createNameTag(unit,tPart[1]);
					unit->resetTarget();
					unit->setHP(500);
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetColours"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",11);
				if(tPart.size()>=10)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)unit->setColours(StringConverter::parseColourValue(tPart[1]),StringConverter::parseColourValue(tPart[2]),StringConverter::parseColourValue(tPart[3]),
												StringConverter::parseColourValue(tPart[4]),StringConverter::parseColourValue(tPart[5]),StringConverter::parseColourValue(tPart[6]),
												StringConverter::parseColourValue(tPart[7]),StringConverter::parseColourValue(tPart[8]),StringConverter::parseColourValue(tPart[9]),
												(tPart.size()>=11?StringConverter::parseColourValue(tPart[10]):ColourValue()),(tPart.size()>=12?StringConverter::parseColourValue(tPart[11]):ColourValue()));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetMarkings"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",3);
				if(tPart.size()>=4)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)unit->setMarkings(StringConverter::parseInt(tPart[1]),StringConverter::parseInt(tPart[2]),StringConverter::parseInt(tPart[3]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetScript"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)
					{
						ScriptedUnit tUnit;
						tUnit.mUnit = unit;
						tUnit.script = tPart[1];
						mDef->loadCampaignScript(tUnit.script,tUnit.nextEvent,tUnit.eventList,0,true);
						if(tUnit.eventList.size()>0)
						{
							processEvent(tUnit.eventList[0],true,&tUnit);
							tUnit.eventList.clear();
							tUnit.eventConditionList.clear();
							mDef->loadCampaignScript(tUnit.script,tUnit.nextEvent,tUnit.eventList);
							for(int i=0;i<(int)tUnit.eventList.size();i++)registerEventConditions(tUnit.eventList[i],&tUnit);
						}
						mScriptedUnits.push_back(tUnit);
					}
				}
			}
			else if(compare(cEvent[i].first,"Unit_RemoveScript"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)
				{
					for(vector<ScriptedUnit>::type::iterator it=mScriptedUnits.begin();it!=mScriptedUnits.end();it++)
					{
						ScriptedUnit *tUnit = &*it;
						if(tUnit->mUnit==unit){mScriptedUnits.erase(it); break;}
					}
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetMaterial"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)
						if(tPart.size()<3)unit->setMaterial(tPart[1]);
						else if(compare(tPart[2],"ENT_BODY"))unit->getBodyEnt()->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_HEAD"))unit->getHeadEnt()->getSubEntity(0)->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_EYES"))unit->getHeadEnt()->getSubEntity(2)->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_TEETH"))unit->getHeadEnt()->getSubEntity(1)->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_WING") && unit->getWingEnt())unit->getWingEnt()->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_TAIL"))unit->getTailEnt()->setMaterialName(tPart[1]);
						else if(compare(tPart[2],"ENT_MANE") && unit->getManeEnt())unit->getManeEnt()->setMaterialName(tPart[1]);
				}
			}
			else if(compare(cEvent[i].first,"Unit_Reset"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)unit->resetTarget();
			}
			else if(compare(cEvent[i].first,"Unit_StopAction"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)unit->stopAction();
			}
			else if(compare(cEvent[i].first,"Unit_SetHP"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setHP(StringConverter::parseReal(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetHPRatio"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setHPRatio(StringConverter::parseReal(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_AddHP"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->addHP(StringConverter::parseReal(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_ClearAttackList"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(!unit)continue;
				if(unit==mUnitManager->getPlayer())mUnitManager->getPlayer()->clearDefaultAttackList();
				else unit->clearAttackList();
			}
			else if(compare(cEvent[i].first,"Unit_PushAttackList"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(!unit)continue;
					if(unit==mUnitManager->getPlayer())
					{
						mUnitManager->getPlayer()->pushDefaultAttackList(tPart[1]);
						mUnitManager->getPlayer()->setAttackListToDefault();
					}
					else
					{
						unit->pushAttackList(tPart[1]);
					}
				}
				
			}
			else if(compare(cEvent[i].first,"Unit_SetAttack"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setAttack(tPart[1]);
				}
			}
			else if(compare(cEvent[i].first,"Unit_ToggleStance"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)unit->toggleStance(mDef);
			}
			else if(compare(cEvent[i].first,"Unit_StopStance"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)unit->stopStance(mDef);
			}
			else if(compare(cEvent[i].first,"Unit_SetEmote"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setEmote(tPart[1],mDef);
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetPosition"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setPosition(parseVector3(tPart[1],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetScale"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->getObjectNode()->setScale(parseVector3(tPart[1],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetLookDirection"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setLookDirection(parseQuaternion(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetYaw"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setYaw(Degree(StringConverter::parseReal(tPart[1])),tPart.size()>=3?StringConverter::parseBool(tPart[2]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetAutoTrackObject"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setAutoTrackObject(parseUnit(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetTarget"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setTarget(parseVector3(tPart[1],sUnit),(tPart.size()>2?parseAction(tPart[2]):TARGET_RUNTO));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetIsWalking"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setIsWalking(StringConverter::parseBool(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetAntiGravity"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setAntiGravity(StringConverter::parseBool(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetFreeLook"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setFreeLook(StringConverter::parseBool(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetAlliance"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setAlliance(parseAlliance(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetCustomAnimation"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",5);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setCustomAnimation(StringConverter::parseBool(tPart[1]),tPart.size()>=3?tPart[2]:"",tPart.size()>=4?StringConverter::parseReal(tPart[3]):1,tPart.size()>=5?StringConverter::parseBool(tPart[4]):false,tPart.size()>=6?StringConverter::parseBool(tPart[5]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_ClampToTerrain"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>0)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)mUnitManager->clampUnitToTerrain(unit,tPart.size()>=2?StringConverter::parseBool(tPart[1]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_SetForce"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setForce(parseVector3(tPart[1],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_AddForce"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->addForce(parseVector3(tPart[1],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_CreateParticle"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()==3)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)mUnitManager->createParticleOnUnit(unit,tPart[1],parseReal(tPart[2]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_CreateParticleOnBone"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",3);
				if(tPart.size()==4)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)mUnitManager->createParticleOnUnitBone(unit,tPart[1],tPart[2],parseReal(tPart[3]));
				}
			}
			
			else if(compare(cEvent[i].first,"Unit_EquipItem"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=3)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)mUnitManager->equipItem(unit,tPart[1],parseItemSlot(tPart[2],unit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_UnequipItem"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)mUnitManager->unequipItem(unit,parseItemSlot(tPart[1],unit));
				}
			}
			else if(compare(cEvent[i].first,"Unit_DoLipSync"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)unit->doLipSync(tPart[1]);
				}
			}
			else if(compare(cEvent[i].first,"Unit_StopLipSync"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second);
				if(unit)unit->stopLipSync();
			}
			else if(compare(cEvent[i].first,"Unit_SetMaxSpeed"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->setMaxSpeed(parseReal(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Unit_DoSit"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=1)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->doSit(tPart.size()>1?StringConverter::parseBool(tPart[1]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_DoSideLay"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=1)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->doSideLay(tPart.size()>1?StringConverter::parseBool(tPart[1]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_DoPrimaryAction"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",3);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->changeAnimBase(tPart[1],tPart.size()>2?parseReal(tPart[2]):1,true,false,tPart.size()>3?StringConverter::parseBool(tPart[3]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_DoRoar"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second,sUnit);
				if(unit)unit->doRoar();
			}
			else if(compare(cEvent[i].first,"Unit_DoSecondaryAction"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",3);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)unit->changeAnimSecond(tPart[1],tPart.size()>2?parseReal(tPart[2]):1,true,false,tPart.size()>3?StringConverter::parseBool(tPart[3]):false);
				}
			}
			else if(compare(cEvent[i].first,"Unit_Destroy"))
			{
				MagixUnit *unit = parseUnit(cEvent[i].second);
				if(unit)
				{
					if(unit==mUnitManager->getPlayer())mUnitManager->destroyUnit(mUnitManager->getPlayer());
					else if(unit->isIndexedUnit())mUnitManager->deleteUnit(static_cast<MagixIndexedUnit*>(unit));
				}
			}
			else if(compare(cEvent[i].first,"Item_Create"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()==3)
				{
					mUnitManager->createItem(StringConverter::parseInt(tPart[0]),tPart[1],parseVector2(tPart[2],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Item_Delete"))mUnitManager->deleteItem(StringConverter::parseInt(cEvent[i].second));
			else if(compare(cEvent[i].first,"Player_ResetSkills"))mUnitManager->getPlayer()->resetSkills();
			else if(compare(cEvent[i].first,"Player_AddSkill"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)mUnitManager->getPlayer()->addSkill(tPart[0],StringConverter::parseInt(tPart[1]));
			}
			else if(compare(cEvent[i].first,"Sound_PlayOnUnit"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)mSoundManager->playSound(tPart[1].c_str(),unit->getObjectNode());
				}
			}
			else if(compare(cEvent[i].first,"Sound_PlayLoopedOnUnit"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)mSoundManager->playLoopedSound(tPart[1].c_str(),unit->getObjectNode());
				}
			}
			else if(compare(cEvent[i].first,"Sound_StopLoopedOnUnit"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=1)
				{
					MagixUnit *unit = parseUnit(tPart[0]);
					if(unit)mSoundManager->stopLoopedSoundByNode(unit->getObjectNode(),tPart.size()>=2?tPart[1].c_str():"");
				}
			}
			else if(compare(cEvent[i].first,"Sound_PlayAmbient"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=1)
				{
					mSoundManager->playAmbientSound(tPart[0].c_str(),tPart.size()>=1?parseReal(tPart[1]):1);
				}
			}
			else if(compare(cEvent[i].first,"Sound_StopAmbient"))mSoundManager->stopAmbientSound();
			else if(compare(cEvent[i].first,"Sound_Play"))mSoundManager->playSound(cEvent[i].second.c_str());
			else if(compare(cEvent[i].first,"Music_Stop"))mSoundManager->stopMusic(StringConverter::parseBool(cEvent[i].second));
			else if(compare(cEvent[i].first,"Music_Play"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)mSoundManager->playMusic(tPart[0].c_str(),tPart.size()>=2?StringConverter::parseBool(tPart[1]):true);
				else mSoundManager->playMusic(cEvent[i].second.c_str());
			}
			else if(compare(cEvent[i].first,"Effect_CreateParticle"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",3);
				if(tPart.size()>=4)mEffectsManager->createParticle(0,tPart[1],parseReal(tPart[3]),false,false,true,parseVector3(tPart[2],sUnit),tPart[0]);
			}
			else if(compare(cEvent[i].first,"Effect_SetParticlePosition"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					ParticleSystem *pSys = mEffectsManager->getParticle(tPart[0]);
					if(pSys)pSys->getParentSceneNode()->setPosition(parseVector3(tPart[1],sUnit));
				}
			}
			else if(compare(cEvent[i].first,"Effect_SetParticleEmissionRate"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>=2)
				{
					ParticleSystem *pSys = mEffectsManager->getParticle(tPart[0]);
					if(pSys)pSys->getEmitter(0)->setEmissionRate(parseReal(tPart[1]));
				}
			}
			else if(compare(cEvent[i].first,"Effect_DestroyParticle"))mEffectsManager->destroyParticle(cEvent[i].second);
			else if(compare(cEvent[i].first,"ToggleCinematic"))toggleCinematic(cEvent[i].second==""?false:StringConverter::parseBool(cEvent[i].second));
			else if(compare(cEvent[i].first,"FadeIn"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>1)mGui->fadeIn(StringConverter::parseReal(tPart[0]),StringConverter::parseColourValue(tPart[1]));
				else mGui->fadeIn(StringConverter::parseReal(cEvent[i].second));
			}
			else if(compare(cEvent[i].first,"FadeOut"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()>1)mGui->fadeOut(StringConverter::parseReal(tPart[0]),StringConverter::parseColourValue(tPart[1]));
				else mGui->fadeOut(StringConverter::parseReal(cEvent[i].second));
			}
			else if(compare(cEvent[i].first,"Progress"))
			{
				progress = StringConverter::parseInt(cEvent[i].second);
				tProgressChanged = true;
			}
			else if(compare(cEvent[i].first,"NextEvent"))
			{
				if(sUnit)sUnit->nextEvent.push_back(cEvent[i].second);
				else nextEvent.push_back(cEvent[i].second);
			}
			else if(compare(cEvent[i].first,"Camera_Reset"))mCamera->reset(false);
			else if(compare(cEvent[i].first,"Camera_ResetCameraTrack"))mCamera->resetCameraTrack(StringConverter::parseReal(cEvent[i].second));
			else if(compare(cEvent[i].first,"Camera_AddCameraTrack"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()>=2)
				{
					const Real tFrame = StringConverter::parseReal(tPart[0]);
					const Vector3 tVect = parseVector3(tPart[1]);
					const Quaternion tQuat = tPart.size()>=3?parseQuaternion(tPart[2]):Quaternion(Degree(180),Vector3::UNIT_Y);
					
					mCamera->addCameraTrackKeyFrame(tFrame,tVect,tQuat);
				}
			}
			else if(compare(cEvent[i].first,"Camera_DoCameraTrack"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",2);
				if(tPart.size()==3)
				{
					MagixUnit *unit = parseUnit(tPart[2]);
					mCamera->doCameraTrack(StringConverter::parseBool(tPart[0]),StringConverter::parseBool(tPart[1]),unit==0?0:unit->getObjectNode());
				}
				else if(tPart.size()>0)mCamera->doCameraTrack(StringConverter::parseBool(tPart[0]));
			}
			else if(compare(cEvent[i].first,"Camera_StopCameraTrack"))mCamera->stopCameraTrack();
			else if(compare(cEvent[i].first,"Timer"))
			{
				if(sUnit)sUnit->timer = parseReal(cEvent[i].second);
				else cinematicTimer = parseReal(cEvent[i].second);
			}
			else if(compare(cEvent[i].first,"ChatBox_Show"))
			{
				if(cEvent[i].second=="" || StringConverter::parseBool(cEvent[i].second))
				{
					mGui->showChatOverlay(true);
					mGui->showChatBox(true,false);
					if(mGui->getChatManager()->getChannel()!=0)mGui->toggleChatChannel();
				}
				else
				{
					mGui->showChatBox(false,false);
					mGui->showChatOverlay(false);
				}
			}
			else if(compare(cEvent[i].first,"ChatBox_SayPlayer"))mGui->getChatManager()->say(mUnitManager,mUnitManager->getPlayer(),cEvent[i].second,CHAT_LOCAL);
			else if(compare(cEvent[i].first,"ChatBox_SayUnit"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)
				{
					MagixUnit *unit = parseUnit(tPart[0],sUnit);
					if(unit)mGui->getChatManager()->say(mUnitManager,unit,tPart[1],CHAT_LOCAL);
				}
			}
			else if(compare(cEvent[i].first,"CreateSpawns"))createSpawns(StringConverter::parseInt(cEvent[i].second));
			else if(compare(cEvent[i].first,"RandomSpawn"))randomSpawn = StringConverter::parseBool(cEvent[i].second);
			else if(compare(cEvent[i].first,"KillSpawns"))killSpawns();
			else if(compare(cEvent[i].first,"CloseCampaign"))closeCampaign(cEvent[i].second==""?GAMESTATE_STARTSCREEN:GAMESTATE_CREDITS,cEvent[i].second);
			else if(compare(cEvent[i].first,"IgnoreGameOver"))ignoreGameOver = StringConverter::parseBool(cEvent[i].second);
			else if(compare(cEvent[i].first,"Cinematic_Caption"))
			{
				const vector<String>::type tPart = StringUtil::split(cEvent[i].second,",",1);
				if(tPart.size()==2)mGui->showCinematicCaption(StringConverter::parseReal(tPart[0]),tPart[1]);
			}
			else if(compare(cEvent[i].first,"ToggleMotionBlur"))mGui->toggleMotionBlur();
			else if(compare(cEvent[i].first,"AttackFrequency") && sUnit)sUnit->attackFrequency=parseReal(cEvent[i].second);
			else if(compare(cEvent[i].first,"ChaseUnit") && sUnit)sUnit->chaseUnit=parseUnit(cEvent[i].second);
		}
		if(!tProgressChanged && !isInitialize && !sUnit)progress++;
	}
	void registerEventConditions(const CampaignEvent &cEvent, ScriptedUnit *sUnit=0)
	{
		vector<const String>::type tLine;
		for(int i=0;i<(int)cEvent.size();i++)
		{
			if(compare(cEvent[i].first,"EventCondition"))tLine.push_back(cEvent[i].second);
		}
		if(sUnit)sUnit->eventConditionList.push_back(tLine);
		else eventConditionList.push_back(tLine);
	}
	void checkEventCondition(ScriptedUnit *sUnit=0)
	{
		const vector<vector<const String>::type>::type tList = sUnit?sUnit->eventConditionList:eventConditionList;
		unsigned short listID = -1;
		for(int i=0;i<(int)tList.size();i++)
		{
			unsigned short trueConditions = 0;
			for(int j=0;j<(int)tList[i].size();j++)
			{
				const vector<String>::type tPart = StringUtil::split(tList[i][j],",",3);
				if(tPart.size()==2)
				{
					if(compare(tPart[0],"PROGRESS") && progress==parseProgress(tPart[1]))trueConditions++;
					else if(compare(tPart[0],"TIMER"))
					{
						if(sUnit)
						{
							if(sUnit->timer==StringConverter::parseReal(tPart[1]))trueConditions++;
						}
						else if(cinematicTimer==StringConverter::parseReal(tPart[1]))trueConditions++;
					}
					else if(compare(tPart[0],"SPAWN_COUNT") && (int)mSpawn.size()==StringConverter::parseInt(tPart[1]))trueConditions++;
					else if(compare(tPart[0],"PROBABILITY") && Math::UnitRandom()<=StringConverter::parseReal(tPart[1]))trueConditions++;
				}
				else if(tPart.size()==3)
				{
					if(compare(tPart[0],"HP_RATIO_LESS_THAN_EQUALS"))
					{
						MagixUnit *unit = parseUnit(tPart[1],sUnit);
						if(unit&&unit->getHPRatio()<=StringConverter::parseReal(tPart[2]))trueConditions++;
					}
				}
				else if(tPart.size()==4)
				{
					if(compare(tPart[0],"UNIT_IN_REGION") && unitIsInRegion(parseUnit(tPart[1],sUnit),parseVector2(tPart[2],sUnit),parseReal(tPart[3])))trueConditions++;
				}
			}
			if(trueConditions==(int)tList[i].size()){listID = i; break;}
		}
		//Advance
		if(sUnit)
		{
			if(listID!=-1 && listID<(int)sUnit->eventList.size())
			{
				sUnit->nextEvent.clear();
				processEvent(sUnit->eventList[listID],false,sUnit);
				sUnit->eventList.clear();
				sUnit->eventConditionList.clear();
				mDef->loadCampaignScript(sUnit->script,sUnit->nextEvent,sUnit->eventList);
				for(int i=0;i<(int)sUnit->eventList.size();i++)registerEventConditions(sUnit->eventList[i],sUnit);
			}
		}
		else
		{
			if(listID!=-1 && listID<(int)eventList.size())
			{
				nextEvent.clear();
				processEvent(eventList[listID]);
				eventList.clear();
				eventConditionList.clear();
				mDef->loadCampaignScript(campaignFilename,nextEvent,eventList,progress+1);
				for(int i=0;i<(int)eventList.size();i++)registerEventConditions(eventList[i]);
			}
		}
	}
	const Real parseReal(const String &data)
	{
		if(StringUtil::match(data,"RANDOM(*)",false))
		{
			String tString = data;
			tString.erase(0,tString.find_first_of("(")+1);
			tString.erase(tString.find_first_of(")"),1);
			const Vector2 tVect = StringConverter::parseVector2(tString);
			return Math::RangeRandom(tVect.x,tVect.y);
		}
		return StringConverter::parseReal(data);
	}
	const unsigned short parseProgress(const String &data)
	{
		if(compare(data,"NEXT"))return progress;
		else return StringConverter::parseInt(data);
	}
	MagixUnit* parseUnit(const String &data, ScriptedUnit *sUnit=0)
	{
		MagixUnit* unit = 0;
		if(compare(data,"PLAYER"))unit = mUnitManager->getPlayer();
		else if(StringUtil::match(data,"BY_NAME(*)",false))
		{
			String tNameString = data;
			tNameString.erase(0,tNameString.find_first_of("(")+1);
			tNameString.erase(tNameString.find_first_of(")"),1);
			unit = mUnitManager->getByName(tNameString);
		}
		else if(compare(data,"THIS_UNIT") && sUnit)unit = sUnit->mUnit;
		return unit;
	}
	const Vector2 parseVector2(const String &data, ScriptedUnit *sUnit=0)
	{
		const Vector3 tVect3 = parseVector3(data,sUnit,true);
		return Vector2(tVect3.x,tVect3.z);
	}
	const Vector3 parseVector3(const String &data, ScriptedUnit *sUnit=0, bool isVector2=false)
	{
		Vector3 tVect = Vector3::ZERO;
		vector<String>::type tPart = StringUtil::split(data,"+");
		if(tPart.size()==0)tPart.push_back(data);
		for(int i=0;i<(int)tPart.size();i++)
		{
			if(compare(tPart[i],"CAMERA_POSITION"))tVect += mCamera->getCamNode()->_getDerivedPosition();
			else if(compare(tPart[i],"PLAYER_POSITION"))tVect += mUnitManager->getPlayer()->getPosition();
			else if(compare(tPart[i],"THIS_UNIT_POSITION") && sUnit)tVect += sUnit->mUnit->getPosition();
			else if(StringUtil::match(tPart[i],"POSITION_BY_NAME(*)",false))
			{
				String tNameString = tPart[i];
				tNameString.erase(0,tNameString.find_first_of("(")+1);
				tNameString.erase(tNameString.find_first_of(")"),1);
				MagixUnit *unit = mUnitManager->getByName(tNameString);
				if(unit)tVect += unit->getPosition();
			}
			else if(StringUtil::match(tPart[i],"(*)",false))
			{
				String tVectString = tPart[i];
				tVectString.erase(0,tVectString.find_first_of("(")+1);
				tVectString.erase(tVectString.find_first_of(")"),1);
				if(isVector2)
				{
					const Vector2 tVect2 = StringConverter::parseVector2(tVectString);
					tVect += Vector3(tVect2.x, 0, tVect2.y);
				}
				else tVect += StringConverter::parseVector3(tVectString);
			}
			else 
			{
				if(isVector2)
				{
					const Vector2 tVect2 = StringConverter::parseVector2(tPart[i]);
					tVect += Vector3(tVect2.x, 0, tVect2.y);
				}
				else tVect += StringConverter::parseVector3(tPart[i]);
			}
		}
		return tVect;
	}
	const Quaternion parseQuaternion(const String &data)
	{
		Quaternion tQuat;
		if(compare(data,"CAMERA_ORIENTATION"))tQuat = mCamera->getCamNode()->_getDerivedOrientation();
		else
		{
			const Vector3 pyr = StringConverter::parseVector3(data);
			tQuat = Quaternion(Degree(pyr.y),Vector3::UNIT_Y)*Quaternion(Degree(pyr.x),Vector3::UNIT_X)*Quaternion(Degree(pyr.z),Vector3::UNIT_Z);
		}
		return tQuat;
	}
	const unsigned short parseAction(const String &data)
	{
		if(compare(data,"TARGET_RUNTO"))return TARGET_RUNTO;
		if(compare(data,"TARGET_LOOKAT"))return TARGET_LOOKAT;
		if(compare(data,"TARGET_PICKUP"))return TARGET_RUNTOPICKUP;
		return TARGET_RUNTO;
	}
	const unsigned short parseAlliance(const String &data)
	{
		if(compare(data,"ALLIANCE_PLAYER"))return ALLIANCE_PLAYER;
		if(compare(data,"ALLIANCE_FRIEND"))return ALLIANCE_FRIEND;
		if(compare(data,"ALLIANCE_ENEMY"))return ALLIANCE_ENEMY;
		return ALLIANCE_PLAYER;
	}
	const short parseItemSlot(const String &data, MagixUnit *unit=0)
	{
		if(unit && compare(data,"NEXT_EMPTY_ITEM_SLOT"))return mUnitManager->getNextEmptyItemSlot(unit);
		if(unit && compare(data,"NEXT_FILLED_ITEM_SLOT"))return mUnitManager->getNextFilledItemSlot(unit);
		return StringConverter::parseInt(data);
	}
};

#endif